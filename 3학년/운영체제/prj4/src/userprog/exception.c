#include "userprog/exception.h"
#include "syscall.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/gdt.h"
#include "userprog/pagedir.h"
#include "threads/malloc.h"
#include "vm/page.h"
#include "vm/frame.h"
#include "vm/swap.h"
#include "filesys/file.h"
#include "filesys/inode.h"
#include "threads/synch.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern struct lock filesys_lock;

/* Number of page faults processed. */
static long long page_fault_cnt;

static void kill (struct intr_frame *);
static void page_fault (struct intr_frame *);

/* Registers handlers for interrupts that can be caused by user
   programs.

   In a real Unix-like OS, most of these interrupts would be
   passed along to the user process in the form of signals, as
   described in [SV-386] 3-24 and 3-25, but we don't implement
   signals.  Instead, we'll make them simply kill the user
   process.

   Page faults are an exception.  Here they are treated the same
   way as other exceptions, but this will need to change to
   implement virtual memory.

   Refer to [IA32-v3a] section 5.15 "Exception and Interrupt
   Reference" for a description of each of these exceptions. */
void
exception_init (void) 
{
  /* These exceptions can be raised explicitly by a user program,
     e.g. via the INT, INT3, INTO, and BOUND instructions.  Thus,
     we set DPL==3, meaning that user programs are allowed to
     invoke them via these instructions. */
  intr_register_int (3, 3, INTR_ON, kill, "#BP Breakpoint Exception");
  intr_register_int (4, 3, INTR_ON, kill, "#OF Overflow Exception");
  intr_register_int (5, 3, INTR_ON, kill,
                     "#BR BOUND Range Exceeded Exception");

  /* These exceptions have DPL==0, preventing user processes from
     invoking them via the INT instruction.  They can still be
     caused indirectly, e.g. #DE can be caused by dividing by
     0.  */
  intr_register_int (0, 0, INTR_ON, kill, "#DE Divide Error");
  intr_register_int (1, 0, INTR_ON, kill, "#DB Debug Exception");
  intr_register_int (6, 0, INTR_ON, kill, "#UD Invalid Opcode Exception");
  intr_register_int (7, 0, INTR_ON, kill,
                     "#NM Device Not Available Exception");
  intr_register_int (11, 0, INTR_ON, kill, "#NP Segment Not Present");
  intr_register_int (12, 0, INTR_ON, kill, "#SS Stack Fault Exception");
  intr_register_int (13, 0, INTR_ON, kill, "#GP General Protection Exception");
  intr_register_int (16, 0, INTR_ON, kill, "#MF x87 FPU Floating-Point Error");
  intr_register_int (19, 0, INTR_ON, kill,
                     "#XF SIMD Floating-Point Exception");

  /* Most exceptions can be handled with interrupts turned on.
     We need to disable interrupts for page faults because the
     fault address is stored in CR2 and needs to be preserved. */
  intr_register_int (14, 0, INTR_OFF, page_fault, "#PF Page-Fault Exception");
}

/* Prints exception statistics. */
void
exception_print_stats (void) 
{
  printf ("Exception: %lld page faults\n", page_fault_cnt);
}

/* Handler for an exception (probably) caused by a user process. */
static void
kill (struct intr_frame *f) 
{
  /* This interrupt is one (probably) caused by a user process.
     For example, the process might have tried to access unmapped
     virtual memory (a page fault).  For now, we simply kill the
     user process.  Later, we'll want to handle page faults in
     the kernel.  Real Unix-like operating systems pass most
     exceptions back to the process via signals, but we don't
     implement them. */
     
  /* The interrupt frame's code segment value tells us where the
     exception originated. */
  switch (f->cs)
    {
    case SEL_UCSEG:
      /* User's code segment, so it's a user exception, as we
         expected.  Kill the user process.  */
      printf ("%s: dying due to interrupt %#04x (%s).\n",
              thread_name (), f->vec_no, intr_name (f->vec_no));
      intr_dump_frame (f);
      thread_exit (); 

    case SEL_KCSEG:
      /* Kernel's code segment, which indicates a kernel bug.
         Kernel code shouldn't throw exceptions.  (Page faults
         may cause kernel exceptions--but they shouldn't arrive
         here.)  Panic the kernel to make the point.  */
      intr_dump_frame (f);
      PANIC ("Kernel bug - unexpected interrupt in kernel"); 

    default:
      /* Some other code segment?  Shouldn't happen.  Panic the
         kernel. */
      printf ("Interrupt %#04x (%s) in unknown segment %04x\n",
             f->vec_no, intr_name (f->vec_no), f->cs);
      thread_exit ();
    }
}

/* Page fault handler.  This is a skeleton that must be filled in
   to implement virtual memory.  Some solutions to project 2 may
   also require modifying this code.

   At entry, the address that faulted is in CR2 (Control Register
   2) and information about the fault, formatted as described in
   the PF_* macros in exception.h, is in F's error_code member.  The
   example code here shows how to parse that information.  You
   can find more information about both of these in the
   description of "Interrupt 14--Page Fault Exception (#PF)" in
   [IA32-v3a] section 5.15 "Exception and Interrupt Reference". */
static void
page_fault (struct intr_frame *f) 
{
  bool not_present;  /* True: not-present page, false: writing r/o page. */
  bool write;        /* True: access was write, false: access was read. */
  bool user;         /* True: access by user, false: access by kernel. */
  void *fault_addr;  /* Fault address. */

  /* Obtain faulting address, the virtual address that was
     accessed to cause the fault.  It may point to code or to
     data.  It is not necessarily the address of the instruction
     that caused the fault (that's f->eip).
     See [IA32-v2a] "MOV--Move to/from Control Registers" and
     [IA32-v3a] 5.15 "Interrupt 14--Page Fault Exception
     (#PF)". */
  asm ("movl %%cr2, %0" : "=r" (fault_addr));

  /* Turn interrupts back on (they were only off so that we could
     be assured of reading CR2 before it changed). */
  intr_enable ();

  /* Count page faults. */
  page_fault_cnt++;

  /* Determine cause. */
  not_present = (f->error_code & PF_P) == 0;
  write = (f->error_code & PF_W) != 0;
  user = (f->error_code & PF_U) != 0;

  (void) not_present;
  (void) write;

  /* Check page fault */
  if (!user || is_kernel_vaddr(fault_addr))
    {
      thread_current()->exit_status = -1;
      thread_exit();
    }

  /* Handle page fault */
  struct thread *t = thread_current ();
  struct page *p;
  void *kpage;
  bool success = false;
  
  /* Save original fault address for stack growth check */
  void *original_fault_addr = fault_addr;
  
  /* Round down to page boundary for SPT lookup */
  void *fault_page_addr = pg_round_down (fault_addr);
  
  /* Check if it's a valid user address */
  if (!is_user_vaddr (fault_page_addr))
    {
      t->exit_status = -1;
      thread_exit ();
    }
  
  /* Find page in SPT */
  p = page_find (&t->spt, fault_page_addr);
  
  /* Stack growth check */
  if (p == NULL)
    {
      /* Check if it's a valid stack growth */
      uintptr_t fault_addr_val = (uintptr_t) original_fault_addr;
      uintptr_t stack_top = (uintptr_t) PHYS_BASE;
      uintptr_t stack_min = stack_top - (8 * 1024 * 1024); /* 8MB limit */
      
      /* Check if fault address is in stack region */
      if (fault_addr_val >= stack_min && fault_addr_val < stack_top)
        {
          /* Check if it's a valid stack access */
          bool can_grow = false;
          
          if (user && f->esp != NULL)
            {
              uintptr_t esp_addr = (uintptr_t) f->esp;
              uintptr_t esp_page = (uintptr_t) pg_round_down (f->esp);
              uintptr_t fault_page = (uintptr_t) pg_round_down (original_fault_addr);
              
              /* Stack growth heuristic based on documentation:
               * - Must be within one page of ESP page (below or above)
               * - If in page below ESP page: allow only if within 32 bytes of ESP
               * - If in ESP page below ESP: must be within 32 bytes (for PUSHA)
               * - If above ESP: always allow (SUB %esp; MOV patterns)
               */
              if (fault_page >= esp_page - PGSIZE && fault_page <= esp_page)
                {
                  if (fault_page < esp_page)
                    {
                      /* Page below ESP page: allow only if within 32 bytes of ESP */
                      /* This allows stack objects that are just below ESP */
                      if (fault_addr_val >= esp_addr - 32)
                        can_grow = true;
                    }
                  else if (fault_addr_val < esp_addr)
                    {
                      /* Same page as ESP, below ESP: must be within 32 bytes */
                      if (fault_addr_val >= esp_addr - 32)
                        can_grow = true;
                    }
                  else
                    {
                      /* Above ESP: always allow (SUB %esp; MOV patterns) */
                      can_grow = true;
                    }
                }
            }
          else if (!user)
            {
              /* Kernel access - deny stack growth */
              can_grow = false;
            }
          else
            {
              /* User access but no ESP - deny for safety */
              can_grow = false;
            }
          
          if (can_grow)
            {
              /* Valid stack growth - allocate page */
              p = page_alloc (fault_page_addr);
              if (p == NULL)
                thread_exit ();
              
              page_set_zero (p);
              p->writable = true;
              
              /* Insert into SPT */
              if (hash_insert (&t->spt, &p->hash_elem) != NULL)
                {
                  /* Page already exists */
                  free (p);
                  thread_exit ();
                }
              
              /* Allocate frame */
              kpage = frame_alloc (PAL_USER | PAL_ZERO, fault_page_addr);
              if (kpage == NULL)
                {
                  hash_delete (&t->spt, &p->hash_elem);
                  free (p);
                  thread_exit ();
                }
              
              /* Install page */
              if (!pagedir_set_page (t->pagedir, fault_page_addr, kpage, true))
                {
                  frame_free (kpage);
                  hash_delete (&t->spt, &p->hash_elem);
                  free (p);
                  thread_exit ();
                }
              
              p->location = PAGE_IN_MEMORY;
              success = true;
            }
        }
      
      if (!success)
        {
          /* Invalid page fault */
          t->exit_status = -1;
          thread_exit ();
        }
    }
  else
    {
      /* Page exists in SPT - try to find shared frame or allocate new one */
      kpage = NULL;
      bool found_shared = false;
      
      /* For read-only file pages, try to find existing shared frame */
      if (p->location == PAGE_IN_FILE && !p->writable && !p->is_mmap && p->file != NULL)
        {
          kpage = frame_find_and_add_shared (p->file, p->file_offset,
                                             fault_page_addr, &found_shared);
        }
      
      /* If no shared frame found or not shareable, allocate new frame */
      if (!found_shared)
        {
      kpage = frame_alloc (PAL_USER, fault_page_addr);
          
      if (kpage == NULL)
        thread_exit ();
      
      /* Load page based on location */
      switch (p->location)
        {
        case PAGE_IN_FILE:
          {
            /* Load from file - need filesys_lock for file operations */
                bool fs_held = lock_held_by_current_thread (&filesys_lock);
                if (!fs_held)
            lock_acquire (&filesys_lock);
            if (file_read_at (p->file, kpage, p->read_bytes, p->file_offset)
                != (int) p->read_bytes)
              {
                    if (!fs_held)
                lock_release (&filesys_lock);
                frame_free (kpage);
                thread_exit ();
              }
                if (!fs_held)
            lock_release (&filesys_lock);
            
            /* Zero the rest */
            memset (kpage + p->read_bytes, 0, p->zero_bytes);
            break;
          }
          
        case PAGE_IN_SWAP:
          {
            /* Load from swap */
            swap_read (p->swap_slot, kpage);
            swap_free (p->swap_slot);
            break;
          }
          
        case PAGE_ZERO:
          {
            /* Zero page */
            memset (kpage, 0, PGSIZE);
            break;
          }
          
        default:
          frame_free (kpage);
          thread_exit ();
            }
        }
      
      /* Install page */
      if (!pagedir_set_page (t->pagedir, fault_page_addr, kpage, p->writable))
        {
          frame_free (kpage);
          thread_exit ();
        }
      
      p->location = PAGE_IN_MEMORY;
      success = true;
    }
  
  if (!success)
    thread_exit ();
}

