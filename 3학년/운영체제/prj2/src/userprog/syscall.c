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
    
  // 매핑된 페이지인지 확인
  if (pagedir_get_page(thread_current()->pagedir, ptr) == NULL)
    {
      thread_current()->exit_status = -1;
      thread_exit();
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

  if (! chk_valid_ptr (buffer))
    thread_exit ();

  if (fd == 1)
    thread_exit ();

  if (fd == 0)
    {
      for(i = 0; i < (int)size; ++i)
        {
          if (! chk_valid_ptr ((char*)buffer + i))
            thread_exit ();
          *(char*)(buffer + i) = input_getc();
        }
      SYS_RETURN (ret, i);
    }
  else if (fd >= 2 && fd < 128)
    {
      struct thread *cur = thread_current ();
      struct file *f = cur->fd_table[fd];

      if (f == NULL)
        SYS_RETURN (ret, -1);

      lock_acquire (&filesys_lock);
      off_t bytes_read = file_read (f, buffer, size);
      lock_release (&filesys_lock);

      SYS_RETURN (ret, bytes_read);
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

      lock_acquire (&filesys_lock);
      off_t bytes_written = file_write (f, buffer, size);
      lock_release (&filesys_lock);

      SYS_RETURN (ret, bytes_written);
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