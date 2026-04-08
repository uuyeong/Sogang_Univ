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
#define STACK_BLOCK   4
#define SYS_ARG_PTR(ESP, IDX) ((uintptr_t) (ESP) + (IDX) * STACK_BLOCK)
#define SYS_RETURN(RET, VALUE) ({ *(RET) = (int) (VALUE) ; return; })
#define SYS_MAX_NUM   25

inline static bool chk_valid_ptr (const void *);
static void syscall_handler (struct intr_frame *);

/* syscalls... */
static void syscall_fibonacci (void *arg_top, int *ret);
static void syscall_max_of_four_int (void *arg_top, int *ret);
static void syscall_halt (void *arg_top, int *ret);
static void syscall_exit (void *arg_top, int *ret);
static void syscall_exec (void *arg_top, int *ret);
static void syscall_wait (void *arg_top, int *ret);
static void syscall_read (void *arg_top, int *ret);
static void syscall_write (void *arg_top, int *ret);

/* syscall table */
static void (*syscall_table[SYS_MAX_NUM]) (void*, int*);
static int esp_fix_val[SYS_MAX_NUM];
static int arg_size[SYS_MAX_NUM];

void syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");

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

  syscall_table[SYS_READ] = syscall_read;
  esp_fix_val[SYS_READ] = 0;
  arg_size[SYS_READ] = STACK_BLOCK * 3;

  syscall_table[SYS_WRITE] = syscall_write;
  esp_fix_val[SYS_WRITE] = 0;
  arg_size[SYS_WRITE] = STACK_BLOCK * 3;
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
      for(i = 0; i < size; ++i)
        {
          if (! chk_valid_ptr ((char*)buffer + i))
            thread_exit ();
          *(char*)(buffer + i) = input_getc();
        }
      SYS_RETURN (ret, i);
    }
  else
    {
      SYS_RETURN (ret, -1);
    }
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
  else
    {
      SYS_RETURN (ret, -1);
    }
}