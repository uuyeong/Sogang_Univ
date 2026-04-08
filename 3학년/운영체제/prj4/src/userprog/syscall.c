#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include <stdint.h>
#include "lib/user/syscall.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "userprog/process.h"
#include "userprog/pagedir.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/synch.h"
#include "vm/page.h"
#include "vm/frame.h"
#include "threads/malloc.h"
#define STACK_BLOCK   4
#define SYS_ARG_PTR(ESP, IDX) ((uintptr_t) (ESP) + (IDX) * STACK_BLOCK)
#define SYS_RETURN(RET, VALUE) ({ *(RET) = (int) (VALUE) ; return; })
#define SYS_MAX_NUM   25

/* File system global lock for synchronization */
struct lock filesys_lock;

inline static bool chk_valid_ptr (const void *);
static void syscall_handler (struct intr_frame *);

/* syscalls... */
static void syscall_fibonacci (void *arg_top, int *ret);
static void syscall_max_of_four_int (void *arg_top, int *ret);
static void syscall_halt (void *arg_top, int *ret);
static void syscall_exit (void *arg_top, int *ret);
static void syscall_exec (void *arg_top, int *ret);
static void syscall_wait (void *arg_top, int *ret);
static void syscall_create (void *arg_top, int *ret);
static void syscall_remove (void *arg_top, int *ret);
static void syscall_open (void *arg_top, int *ret);
static void syscall_close (void *arg_top, int *ret);
static void syscall_filesize (void *arg_top, int *ret);
static void syscall_read (void *arg_top, int *ret);
static void syscall_write (void *arg_top, int *ret);
static void syscall_seek (void *arg_top, int *ret);
static void syscall_tell (void *arg_top, int *ret);
static void syscall_mmap (void *arg_top, int *ret);
static void syscall_munmap (void *arg_top, int *ret);
static bool copy_from_user (void *dst, const void *src, size_t size);
static bool copy_to_user (void *dst, const void *src, size_t size);

/* syscall table */
static void (*syscall_table[SYS_MAX_NUM]) (void*, int*);
static int esp_fix_val[SYS_MAX_NUM];
static int arg_size[SYS_MAX_NUM];

void syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  
  /* Initialize file system lock */
  lock_init (&filesys_lock);

  syscall_table[SYS_FIBONACCI] = syscall_fibonacci;
  esp_fix_val[SYS_FIBONACCI] = 0;
  arg_size[SYS_FIBONACCI] = STACK_BLOCK * 1;
  
  syscall_table[SYS_MAX_OF_FOUR_INT] = syscall_max_of_four_int;
  esp_fix_val[SYS_MAX_OF_FOUR_INT] = 0;
  arg_size[SYS_MAX_OF_FOUR_INT] = STACK_BLOCK * 4;
  
  syscall_table[SYS_HALT] = syscall_halt;
  esp_fix_val[SYS_HALT] = 0;
  arg_size[SYS_HALT] = STACK_BLOCK * 0;

  syscall_table[SYS_EXIT] = syscall_exit;
  esp_fix_val[SYS_EXIT] = 0;
  arg_size[SYS_EXIT] = STACK_BLOCK * 1;

  syscall_table[SYS_EXEC] = syscall_exec;
  esp_fix_val[SYS_EXEC] = 0;
  arg_size[SYS_EXEC] = STACK_BLOCK * 1;
  
  syscall_table[SYS_WAIT] = syscall_wait;
  esp_fix_val[SYS_WAIT] = 0;
  arg_size[SYS_WAIT] = STACK_BLOCK * 1;

  syscall_table[SYS_CREATE] = syscall_create;
  esp_fix_val[SYS_CREATE] = 0;
  arg_size[SYS_CREATE] = STACK_BLOCK * 2;

  syscall_table[SYS_REMOVE] = syscall_remove;
  esp_fix_val[SYS_REMOVE] = 0;
  arg_size[SYS_REMOVE] = STACK_BLOCK * 1;

  syscall_table[SYS_OPEN] = syscall_open;
  esp_fix_val[SYS_OPEN] = 0;
  arg_size[SYS_OPEN] = STACK_BLOCK * 1;

  syscall_table[SYS_CLOSE] = syscall_close;
  esp_fix_val[SYS_CLOSE] = 0;
  arg_size[SYS_CLOSE] = STACK_BLOCK * 1;

  syscall_table[SYS_FILESIZE] = syscall_filesize;
  esp_fix_val[SYS_FILESIZE] = 0;
  arg_size[SYS_FILESIZE] = STACK_BLOCK * 1;

  syscall_table[SYS_READ] = syscall_read;
  esp_fix_val[SYS_READ] = 0;
  arg_size[SYS_READ] = STACK_BLOCK * 3;

  syscall_table[SYS_WRITE] = syscall_write;
  esp_fix_val[SYS_WRITE] = 0;
  arg_size[SYS_WRITE] = STACK_BLOCK * 3;

  syscall_table[SYS_SEEK] = syscall_seek;
  esp_fix_val[SYS_SEEK] = 0;
  arg_size[SYS_SEEK] = STACK_BLOCK * 2;

  syscall_table[SYS_TELL] = syscall_tell;
  esp_fix_val[SYS_TELL] = 0;
  arg_size[SYS_TELL] = STACK_BLOCK * 1;

  syscall_table[SYS_MMAP] = syscall_mmap;
  esp_fix_val[SYS_MMAP] = 0;
  arg_size[SYS_MMAP] = STACK_BLOCK * 2;

  syscall_table[SYS_MUNMAP] = syscall_munmap;
  esp_fix_val[SYS_MUNMAP] = 0;
  arg_size[SYS_MUNMAP] = STACK_BLOCK * 1;
}

inline static bool
chk_valid_ptr (const void *ptr)
{
  if (ptr == NULL)
    {
      thread_current()->exit_status = -1;
      thread_exit();
    }
  
  if (!is_user_vaddr(ptr))
    {
      thread_current()->exit_status = -1;
      thread_exit();
    }
    
  // Virtual memory에서는 pagedir_get_page로 확인하면 안됨
  // Demand paging으로 인해 아직 매핑되지 않은 페이지일 수 있음
  // 대신 user virtual address인지만 확인
    
  return true;
}

static bool
copy_from_user (void *dst, const void *src, size_t size)
{
  uint8_t *d = dst;
  const uint8_t *s = src;
  struct thread *cur = thread_current ();

  for (size_t i = 0; i < size; i++)
    {
      const void *addr = s + i;
      if (!is_user_vaddr (addr))
        return false;

      void *upage = pg_round_down (addr);
      if (pagedir_get_page (cur->pagedir, upage) == NULL)
        {
          if (process_ensure_page ((void *) addr) == NULL)
            return false;
        }

      d[i] = *(volatile uint8_t *) addr;
    }
  return true;
}

static bool
copy_to_user (void *dst, const void *src, size_t size)
{
  uint8_t *d = dst;
  const uint8_t *s = src;
  struct thread *cur = thread_current ();

  for (size_t i = 0; i < size; i++)
    {
      void *addr = d + i;
      if (!is_user_vaddr (addr))
        return false;

      void *upage = pg_round_down (addr);
      if (pagedir_get_page (cur->pagedir, upage) == NULL)
        {
          if (process_ensure_page (addr) == NULL)
            return false;
        }

      *(volatile uint8_t *) addr = s[i];
    }
  return true;
}

static void
syscall_handler (struct intr_frame *f)
{
  uint32_t syscall_num;

  syscall_num = * (uint32_t *) f->esp;

  if (syscall_num >= SYS_MAX_NUM || syscall_table[syscall_num] == NULL)
    thread_exit ();
  else
    {
      void *arg_top;
      
      arg_top = (void*) ((uintptr_t) f->esp + STACK_BLOCK + esp_fix_val[syscall_num]);

      if (! is_user_vaddr((void*) ((uintptr_t) arg_top + arg_size[syscall_num] - STACK_BLOCK)))
        thread_exit ();

      syscall_table[syscall_num](arg_top, &f->eax);
    }
}

static void 
syscall_fibonacci (void *arg_top, int *ret)
{
  int n = * (int *) SYS_ARG_PTR (arg_top, 0);

  int a, b, c, i;

  a = 0; b = c = 1;

  if(n == 0) 
    SYS_RETURN (ret, 0);

  for(i = 1; i < n; ++i)
    {
      c = a + b;
      a = b;
      b = c;
    }

  SYS_RETURN (ret, c);
}

static void
syscall_max_of_four_int (void *arg_top, int *ret)
{
  int a = * (int *) SYS_ARG_PTR (arg_top, 0);
  int b = * (int *) SYS_ARG_PTR (arg_top, 1);
  int c = * (int *) SYS_ARG_PTR (arg_top, 2);
  int d = * (int *) SYS_ARG_PTR (arg_top, 3);

  int max = a;
  if (b > max) max = b;
  if (c > max) max = c;
  if (d > max) max = d;

  SYS_RETURN (ret, max);
}

static void 
syscall_halt (void *arg_top UNUSED, int *ret UNUSED)
{
  shutdown_power_off();
}

static void
syscall_exit (void *arg_top, int *ret UNUSED)
{
  int status = * (int *) SYS_ARG_PTR (arg_top, 0);

  struct thread *cur = thread_current ();
  cur->exit_status = status;
  thread_exit();
}

static void
syscall_exec (void *arg_top, int *ret)
{
  const char *file = * (char **) SYS_ARG_PTR (arg_top, 0);

  if (! chk_valid_ptr (file))
    SYS_RETURN (ret, -1);

  SYS_RETURN ( ret, process_execute(file) ); 
}

static void
syscall_wait (void *arg_top, int *ret)
{
  pid_t pid = * (pid_t *) SYS_ARG_PTR (arg_top, 0); 

  SYS_RETURN ( ret, process_wait(pid) );
}

static void
syscall_read (void *arg_top, int *ret)
{
  int fd = * (int *) SYS_ARG_PTR (arg_top, 0);
  void *buffer = * (void **) SYS_ARG_PTR (arg_top, 1);
  unsigned size = * (unsigned *) SYS_ARG_PTR (arg_top, 2);

  int i;
  struct thread *cur = thread_current ();

  if (! chk_valid_ptr (buffer))
    thread_exit ();

  if (fd == 1)
    thread_exit ();

  /* Validate buffer address range */
  for (i = 0; i < (int)size; i++)
    {
      void *addr = (char*)buffer + i;
      if (! chk_valid_ptr (addr))
        thread_exit ();
      
      void *upage = pg_round_down (addr);
      void *kpage = pagedir_get_page (cur->pagedir, upage);
      struct page *p = page_find (&cur->spt, upage);
      
      if (kpage == NULL && p == NULL)
        {
          /* Invalid address - not mapped and not in SPT */
          cur->exit_status = -1;
          thread_exit ();
        }

      if (kpage == NULL)
        {
          if (process_ensure_page (addr) == NULL)
            thread_exit ();
          kpage = pagedir_get_page (cur->pagedir, upage);
          p = page_find (&cur->spt, upage);
        }
      
      /* Check if it's a code segment (read-only, non-writable page) */
      if (p != NULL && !p->writable)
        {
          /* Code segment - deny write access (read syscall writes to buffer) */
          cur->exit_status = -1;
          thread_exit ();
        }
      
      /* If page is mapped, check if it's writable */
      if (kpage != NULL)
        {
          /* Check if page is writable by checking SPT */
          if (p != NULL && !p->writable)
            {
              /* Code segment - deny write access */
              cur->exit_status = -1;
              thread_exit ();
            }
        }
    }

  if (fd == 0)
    {
      for(i = 0; i < (int)size; ++i)
        {
          *(char*)(buffer + i) = input_getc();
        }
      SYS_RETURN (ret, i);
    }
  else if (fd >= 2 && fd < 128)
    {
      struct file *f = cur->fd_table[fd];

      if (f == NULL)
        SYS_RETURN (ret, -1);

      uint8_t *bounce = malloc (PGSIZE);
      if (bounce == NULL)
        SYS_RETURN (ret, -1);

      off_t total_read = 0;
      uint8_t *dst = buffer;
      size_t remaining = size;

      while (remaining > 0)
        {
          size_t chunk = remaining < PGSIZE ? remaining : PGSIZE;

      lock_acquire (&filesys_lock);
          off_t bytes_read = file_read (f, bounce, chunk);
      lock_release (&filesys_lock);

          if (bytes_read <= 0)
            break;

          if (!copy_to_user (dst, bounce, bytes_read))
            {
              free (bounce);
              thread_exit ();
            }

          dst += bytes_read;
          total_read += bytes_read;
          remaining -= bytes_read;

          if ((size_t) bytes_read < chunk)
            break;
        }

      free (bounce);
      SYS_RETURN (ret, total_read);
    }
  else
    {
      SYS_RETURN (ret, -1);
    }
}

static void
syscall_create (void *arg_top, int *ret)
{
  const char *file = * (char **) SYS_ARG_PTR (arg_top, 0);
  unsigned initial_size = * (unsigned *) SYS_ARG_PTR (arg_top, 1);

  if (! chk_valid_ptr (file))
    SYS_RETURN (ret, false);

  lock_acquire (&filesys_lock);
  bool success = filesys_create (file, initial_size);
  lock_release (&filesys_lock);

  SYS_RETURN (ret, success);
}

static void
syscall_remove (void *arg_top, int *ret)
{
  const char *file = * (char **) SYS_ARG_PTR (arg_top, 0);

  if (! chk_valid_ptr (file))
    SYS_RETURN (ret, false);

  lock_acquire (&filesys_lock);
  bool success = filesys_remove (file);
  lock_release (&filesys_lock);

  SYS_RETURN (ret, success);
}

static void
syscall_open (void *arg_top, int *ret)
{
  const char *file = * (char **) SYS_ARG_PTR (arg_top, 0);

  if (! chk_valid_ptr (file))
    SYS_RETURN (ret, -1);

  lock_acquire (&filesys_lock);
  struct file *f = filesys_open (file);
  lock_release (&filesys_lock);

  if (f == NULL)
    SYS_RETURN (ret, -1);

  struct thread *cur = thread_current ();
  int fd = cur->next_fd;
  
  /* Find available fd slot */
  while (fd < 128 && cur->fd_table[fd] != NULL)
    fd++;

  if (fd >= 128)
    {
      lock_acquire (&filesys_lock);
      file_close (f);
      lock_release (&filesys_lock);
      SYS_RETURN (ret, -1);
    }

  cur->fd_table[fd] = f;
  cur->next_fd = fd + 1;

  SYS_RETURN (ret, fd);
}

static void
syscall_close (void *arg_top, int *ret UNUSED)
{
  int fd = * (int *) SYS_ARG_PTR (arg_top, 0);

  if (fd < 2 || fd >= 128)
    return;

  struct thread *cur = thread_current ();
  struct file *f = cur->fd_table[fd];

  if (f == NULL)
    return;

  cur->fd_table[fd] = NULL;

  lock_acquire (&filesys_lock);
  file_close (f);
  lock_release (&filesys_lock);
}

static void
syscall_filesize (void *arg_top, int *ret)
{
  int fd = * (int *) SYS_ARG_PTR (arg_top, 0);

  if (fd < 2 || fd >= 128)
    SYS_RETURN (ret, -1);

  struct thread *cur = thread_current ();
  struct file *f = cur->fd_table[fd];

  if (f == NULL)
    SYS_RETURN (ret, -1);

  lock_acquire (&filesys_lock);
  off_t size = file_length (f);
  lock_release (&filesys_lock);

  SYS_RETURN (ret, size);
}

static void
syscall_write (void *arg_top, int *ret)
{  
  int fd = * (int *) SYS_ARG_PTR (arg_top, 0);
  void *buffer = * (void **) SYS_ARG_PTR (arg_top, 1);
  unsigned size = * (unsigned *) SYS_ARG_PTR (arg_top, 2);

  if (! chk_valid_ptr (buffer))
    thread_exit ();

  if (fd == 0 || buffer == NULL)
    thread_exit ();

  if (fd == 1)
    {
      putbuf (buffer, size);
      SYS_RETURN (ret, size);
    }
  else if (fd >= 2 && fd < 128)
    {
      struct thread *cur = thread_current ();
      struct file *f = cur->fd_table[fd];

      if (f == NULL)
        SYS_RETURN (ret, -1);

      uint8_t *bounce = malloc (PGSIZE);
      if (bounce == NULL)
        SYS_RETURN (ret, -1);

      off_t total_written = 0;
      const uint8_t *src = buffer;
      size_t remaining = size;

      while (remaining > 0)
        {
          size_t chunk = remaining < PGSIZE ? remaining : PGSIZE;

          if (!copy_from_user (bounce, src, chunk))
            {
              free (bounce);
              thread_exit ();
            }

      lock_acquire (&filesys_lock);
          off_t bytes_written = file_write (f, bounce, chunk);
      lock_release (&filesys_lock);

          if (bytes_written <= 0)
            break;

          src += bytes_written;
          total_written += bytes_written;
          remaining -= bytes_written;

          if ((size_t) bytes_written < chunk)
            break;
        }

      free (bounce);
      SYS_RETURN (ret, total_written);
    }
  else
    {
      SYS_RETURN (ret, -1);
    }
}

static void
syscall_seek (void *arg_top, int *ret UNUSED)
{
  int fd = * (int *) SYS_ARG_PTR (arg_top, 0);
  unsigned position = * (unsigned *) SYS_ARG_PTR (arg_top, 1);

  if (fd < 2 || fd >= 128)
    return;

  struct thread *cur = thread_current ();
  struct file *f = cur->fd_table[fd];

  if (f == NULL)
    return;

  lock_acquire (&filesys_lock);
  file_seek (f, position);
  lock_release (&filesys_lock);
}

static void
syscall_tell (void *arg_top, int *ret)
{
  int fd = * (int *) SYS_ARG_PTR (arg_top, 0);

  if (fd < 2 || fd >= 128)
    SYS_RETURN (ret, -1);

  struct thread *cur = thread_current ();
  struct file *f = cur->fd_table[fd];

  if (f == NULL)
    SYS_RETURN (ret, -1);

  lock_acquire (&filesys_lock);
  unsigned position = file_tell (f);
  lock_release (&filesys_lock);

  SYS_RETURN (ret, position);
}

static void
syscall_mmap (void *arg_top, int *ret)
{
  int fd = * (int *) SYS_ARG_PTR (arg_top, 0);
  void *addr = * (void **) SYS_ARG_PTR (arg_top, 1);
  
  if (! chk_valid_ptr (addr))
    SYS_RETURN (ret, -1);
  
  if (fd < 2 || fd >= 128)
    SYS_RETURN (ret, -1);
  
  struct thread *cur = thread_current ();
  struct file *f = cur->fd_table[fd];
  
  if (f == NULL)
    SYS_RETURN (ret, -1);
  
  /* Check alignment */
  if ((uintptr_t) addr % PGSIZE != 0)
    SYS_RETURN (ret, -1);
  
  /* Check if address is in user space */
  if (!is_user_vaddr (addr))
    SYS_RETURN (ret, -1);
  
  /* Get file size */
  lock_acquire (&filesys_lock);
  off_t file_size = file_length (f);
  lock_release (&filesys_lock);
  
  if (file_size == 0)
    SYS_RETURN (ret, -1);
  
  /* Calculate number of pages */
  size_t page_count = (file_size + PGSIZE - 1) / PGSIZE;
  void *upage = addr;
  off_t offset = 0;
  
  /* Create mmap entries in SPT */
  for (size_t i = 0; i < page_count; i++)
    {
      struct page *p = page_alloc (upage);
      if (p == NULL)
        {
          /* Cleanup on failure */
          for (size_t j = 0; j < i; j++)
            {
              void *cleanup_page = (uint8_t *) addr + j * PGSIZE;
              struct page *cleanup_p = page_find (&cur->spt, cleanup_page);
              if (cleanup_p != NULL)
                page_free (&cur->spt, cleanup_p);
            }
          SYS_RETURN (ret, -1);
        }
      
      /* Set mmap information */
      size_t read_bytes = (offset + PGSIZE <= file_size) ? PGSIZE : (file_size - offset);
      size_t zero_bytes = PGSIZE - read_bytes;
      
      p->location = PAGE_IN_FILE;
      p->file = file_reopen (f);
      p->file_offset = offset;
      p->read_bytes = read_bytes;
      p->zero_bytes = zero_bytes;
      p->writable = true;
      p->is_mmap = true;
      p->mmap_file = p->file;
      p->mmap_offset = offset;
      
      /* Insert into SPT */
      if (hash_insert (&cur->spt, &p->hash_elem) != NULL)
        {
          free (p);
          /* Cleanup */
          for (size_t j = 0; j < i; j++)
            {
              void *cleanup_page = (uint8_t *) addr + j * PGSIZE;
              struct page *cleanup_p = page_find (&cur->spt, cleanup_page);
              if (cleanup_p != NULL)
                {
                  if (cleanup_p->mmap_file != NULL)
                    file_close (cleanup_p->mmap_file);
                  page_free (&cur->spt, cleanup_p);
                }
            }
          SYS_RETURN (ret, -1);
        }
      
      upage += PGSIZE;
      offset += PGSIZE;
    }
  
  SYS_RETURN (ret, (int) addr);
}

static void
syscall_munmap (void *arg_top, int *ret UNUSED)
{
  void *addr = * (void **) SYS_ARG_PTR (arg_top, 0);
  
  if (! chk_valid_ptr (addr))
    return;
  
  if ((uintptr_t) addr % PGSIZE != 0)
    return;
  
  struct thread *cur = thread_current ();
  void *upage = addr;
  
  /* Find and unmap all pages starting from addr */
  while (true)
    {
      struct page *p = page_find (&cur->spt, upage);
      
      if (p == NULL || !p->is_mmap)
        break;
      
      /* Write back if dirty */
      void *kpage = pagedir_get_page (cur->pagedir, upage);
      if (kpage != NULL)
        {
          bool dirty = pagedir_is_dirty (cur->pagedir, upage);
          if (dirty && p->mmap_file != NULL)
            {
              lock_acquire (&filesys_lock);
              file_write_at (p->mmap_file, kpage, p->read_bytes, p->mmap_offset);
              lock_release (&filesys_lock);
            }
          
          pagedir_clear_page (cur->pagedir, upage);
          frame_free (kpage);
        }
      
      /* Close file if it's the last reference */
      if (p->mmap_file != NULL)
        {
          lock_acquire (&filesys_lock);
          file_close (p->mmap_file);
          lock_release (&filesys_lock);
        }
      
      /* Remove from SPT */
      page_free (&cur->spt, p);
      
      upage += PGSIZE;
    }
}