#include "vm/frame.h"
#include "vm/page.h"
#include "vm/swap.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "threads/thread.h"
#include "userprog/pagedir.h"
#include "filesys/file.h"
#include "filesys/inode.h"
#include "lib/kernel/list.h"
#include "debug.h"

extern struct lock filesys_lock;

/* Frame table - global list of all frames */
struct list frame_table;
struct lock frame_lock;

static void
frame_lock_acquire_debug (const char *file, int line)
{
  if (lock_held_by_current_thread (&frame_lock))
    PANIC ("frame_lock already held (%s:%d)", file, line);
  lock_acquire (&frame_lock);
}

#define FRAME_LOCK_ACQUIRE() frame_lock_acquire_debug (__FILE__, __LINE__)
#define FRAME_LOCK_RELEASE() lock_release (&frame_lock)

/* Second Chance Algorithm - clock hand */
static struct list_elem *clock_hand;

/* Initialize frame table */
void
frame_init (void)
{
  list_init (&frame_table);
  lock_init (&frame_lock);
  clock_hand = NULL;
}

/* Allocate a frame for a user page */
void *
frame_alloc (enum palloc_flags flags, void *upage)
{
  void *kpage;
  struct frame *f;
  struct thread *t = thread_current ();
  struct page *p;
  
  FRAME_LOCK_ACQUIRE ();
  
  kpage = palloc_get_page (flags);
  if (kpage == NULL)
    {
      FRAME_LOCK_RELEASE ();
      kpage = frame_evict ();
      if (kpage == NULL)
          return NULL;
      FRAME_LOCK_ACQUIRE ();
    }
  
  f = malloc (sizeof (struct frame));
  if (f == NULL)
    {
      FRAME_LOCK_RELEASE ();
      palloc_free_page (kpage);
      return NULL;
    }
  
  f->kpage = kpage;
  f->upage = upage;
  f->thread = t;
  f->pinned = false;
  
  /* Get file and offset information from SPT for page merging */
  /* SPT is per-process, so no lock needed for access */
  p = page_find (&t->spt, upage);
  f->file = NULL;
  f->file_offset = 0;
  f->read_only = false;

  if (p != NULL && p->file != NULL && !p->writable && !p->is_mmap)
    {
      f->file = p->file;
      f->file_offset = p->file_offset;
      f->read_only = true;
    }
  
  list_push_back (&frame_table, &f->elem);
  if (clock_hand == NULL)
    clock_hand = list_begin (&frame_table);

  FRAME_LOCK_RELEASE ();
  return kpage;
}

/* Allocate a frame with file and offset information (for page merging) */
void *
frame_alloc_with_file (enum palloc_flags flags, void *upage, struct file *file, off_t offset)
{
  void *kpage;
  struct frame *f;
  struct thread *t = thread_current ();

      FRAME_LOCK_ACQUIRE ();

  kpage = palloc_get_page (flags);
  if (kpage == NULL)
    {
      FRAME_LOCK_RELEASE ();
      kpage = frame_evict ();
      if (kpage == NULL)
        return NULL;
      FRAME_LOCK_ACQUIRE ();
    }

  f = malloc (sizeof (struct frame));
  if (f == NULL)
    {
      FRAME_LOCK_RELEASE ();
      palloc_free_page (kpage);
      return NULL;
    }
  
  f->kpage = kpage;
  f->upage = upage;
  f->thread = t;
  f->pinned = false;
  
  /* Set file and offset information directly */
  if (file != NULL)
    {
      f->file = file;
      f->file_offset = offset;
      f->read_only = true;
    }
  else
    {
      f->file = NULL;
      f->file_offset = 0;
      f->read_only = false;
    }
  
  list_push_back (&frame_table, &f->elem);
  if (clock_hand == NULL)
    clock_hand = list_begin (&frame_table);

  FRAME_LOCK_RELEASE ();
  return kpage;
}

/* Free a frame */
void
frame_free (void *kpage)
{
  struct frame *f;
  struct list_elem *e;
  int ref_count = 0;
  
  if (kpage == NULL)
    return;
  
  FRAME_LOCK_ACQUIRE ();
  
  /* Count references to this frame (for page merging) */
  for (e = list_begin (&frame_table); e != list_end (&frame_table); e = list_next (e))
    {
      struct frame *frame = list_entry (e, struct frame, elem);
      if (frame->kpage == kpage)
        ref_count++;
    }
  
  /* Only free if this is the last reference */
  if (ref_count <= 1)
    {
      /* Find and remove all frames with this kpage */
      e = list_begin (&frame_table);
      while (e != list_end (&frame_table))
        {
          f = list_entry (e, struct frame, elem);
          e = list_next (e);
          
          if (f->kpage == kpage)
    {
      /* Remove from frame table */
      if (clock_hand == &f->elem)
                {
                  clock_hand = list_next (&f->elem);
                  if (clock_hand == list_end (&frame_table))
                    clock_hand = list_begin (&frame_table);
                }
      list_remove (&f->elem);
              
              /* Free frame entry */
              free (f);
            }
        }
      
      /* Free physical page */
      palloc_free_page (kpage);
    }
  else
    {
      /* Multiple references - find the frame for current thread's page */
      struct thread *t = thread_current ();
      f = NULL;
      
      for (e = list_begin (&frame_table); e != list_end (&frame_table); e = list_next (e))
        {
          struct frame *frame = list_entry (e, struct frame, elem);
          if (frame->kpage == kpage && frame->thread == t)
            {
              f = frame;
              break;
            }
        }
      
      if (f != NULL)
        {
          /* Remove from frame table */
          if (clock_hand == &f->elem)
            {
              clock_hand = list_next (clock_hand);
              if (clock_hand == list_end (&frame_table))
                clock_hand = list_begin (&frame_table);
            }
          list_remove (&f->elem);
      
      /* Free frame entry */
      free (f);
        }
    }
  
  FRAME_LOCK_RELEASE ();
}

/* Find frame by kernel page address */
struct frame *
frame_find (void *kpage)
{
  struct list_elem *e;

  FRAME_LOCK_ACQUIRE ();
  
  for (e = list_begin (&frame_table); e != list_end (&frame_table); e = list_next (e))
    {
      struct frame *f = list_entry (e, struct frame, elem);
      if (f->kpage == kpage)
        {
          FRAME_LOCK_RELEASE ();
        return f;
        }
    }
  
  FRAME_LOCK_RELEASE ();
  return NULL;
}

/* Add a frame entry for a shared page */
/* NOTE: This function always acquires and releases frame_lock internally */
bool
frame_add_entry (void *kpage, void *upage)
{
  struct frame *f;
  struct frame *shared_f;
  struct list_elem *e;
  struct thread *t = thread_current ();
  struct page *p;
  
  f = malloc (sizeof (struct frame));
  if (f == NULL)
    return false;
  
  f->kpage = kpage;
  f->upage = upage;
  f->thread = t;
  f->pinned = false;
  
  FRAME_LOCK_ACQUIRE ();
  
  /* Find the shared frame to get file/offset info */
  shared_f = NULL;
  for (e = list_begin (&frame_table); e != list_end (&frame_table); e = list_next (e))
    {
      struct frame *check_f = list_entry (e, struct frame, elem);
      if (check_f->kpage == kpage)
        {
          shared_f = check_f;
          break;
        }
    }
  
  if (shared_f != NULL && shared_f->file != NULL)
    {
      f->file = shared_f->file;
      f->file_offset = shared_f->file_offset;
      f->read_only = shared_f->read_only;
    }
  else
    {
      /* Fallback: get from SPT */
      /* SPT is per-process, so no lock needed for access */
      p = page_find (&t->spt, upage);
      if (p != NULL && p->file != NULL && !p->writable && !p->is_mmap)
        {
          f->file = p->file;
          f->file_offset = p->file_offset;
          f->read_only = true;
        }
      else
        {
          f->file = NULL;
          f->file_offset = 0;
          f->read_only = false;
        }
    }
  
  list_push_back (&frame_table, &f->elem);
  
  FRAME_LOCK_RELEASE ();
  
  return true;
}

/* Find shared frame and add entry atomically (for page merging) */
void *
frame_find_and_add_shared (struct file *file, off_t offset, void *upage, bool *found)
{
  struct list_elem *e;
  struct inode *target_inode;
  block_sector_t target_sector;
  struct frame *found_frame = NULL;
  void *kpage = NULL;
  
  if (file == NULL || found == NULL)
    return NULL;
  
  *found = false;
  
  target_inode = file_get_inode (file);
  if (target_inode == NULL)
    return NULL;
  
  target_sector = inode_get_inumber (target_inode);
  
  FRAME_LOCK_ACQUIRE ();
  
  /* Search for shared frame */
  for (e = list_begin (&frame_table); e != list_end (&frame_table); e = list_next (e))
    {
      struct frame *f = list_entry (e, struct frame, elem);
      
      if (f->file != NULL && f->read_only && f->file_offset == offset)
        {
          struct inode *frame_inode = file_get_inode (f->file);
          if (frame_inode != NULL)
            {
              block_sector_t frame_sector = inode_get_inumber (frame_inode);
              
              if (frame_sector == target_sector)
                {
                  found_frame = f;
                  kpage = f->kpage;
                  break;
                }
            }
        }
    }
  
  /* If found, add new frame entry */
  if (found_frame != NULL)
    {
      struct frame *new_f = malloc (sizeof (struct frame));
      if (new_f != NULL)
        {
          new_f->kpage = kpage;
          new_f->upage = upage;
          new_f->thread = thread_current ();
          new_f->pinned = false;
          new_f->file = found_frame->file;
          new_f->file_offset = found_frame->file_offset;
          new_f->read_only = found_frame->read_only;
          
          list_push_back (&frame_table, &new_f->elem);
          *found = true;
        }
      else
        {
          kpage = NULL;
        }
    }
  
  FRAME_LOCK_RELEASE ();
  
  return kpage;
}

/* Find shared frame by file and offset for page merging */
/* NOTE: This function always acquires and releases frame_lock internally */
void *
frame_find_shared (struct file *file, off_t offset)
{
  struct list_elem *e;
  struct inode *target_inode;
  block_sector_t target_sector;
  struct frame *found_frame = NULL;
  
  if (file == NULL)
    return NULL;
  
  target_inode = file_get_inode (file);
  if (target_inode == NULL)
    return NULL;
  
  target_sector = inode_get_inumber (target_inode);
  
  FRAME_LOCK_ACQUIRE ();
  
  /* Search through global frame table to find matching frame */
  /* This is the key: we search ALL frames, not just current process's frames */
  /* Frames in frame_table are guaranteed to be in physical memory */
  for (e = list_begin (&frame_table); e != list_end (&frame_table); e = list_next (e))
    {
      struct frame *f = list_entry (e, struct frame, elem);
      
      /* Check if frame has file and offset information (for page merging) */
      if (f->file != NULL && f->read_only && f->file_offset == offset)
        {
          struct inode *frame_inode = file_get_inode (f->file);
          if (frame_inode != NULL)
            {
              block_sector_t frame_sector = inode_get_inumber (frame_inode);
              
              /* Match same file (inode) and same offset */
              /* Use inode comparison, not file pointer comparison */
              if (frame_sector == target_sector)
                {
                  /* Frame is in physical memory (it's in frame_table) */
                  /* No need to check page directory - frame_table is authoritative */
                  found_frame = f;
                  break;
                }
            }
        }
    }
  
  FRAME_LOCK_RELEASE ();
  
  return found_frame != NULL ? found_frame->kpage : NULL;
}

/* Find frame by file and offset for page merging (internal use) */

/* Set frame pinned status */
void
frame_set_pinned (void *kpage, bool pinned)
{
  struct frame *f;
  struct list_elem *e;
  
  FRAME_LOCK_ACQUIRE ();
  
  /* Find frame directly without calling frame_find (which also acquires lock) */
  for (e = list_begin (&frame_table); e != list_end (&frame_table); e = list_next (e))
    {
      f = list_entry (e, struct frame, elem);
      if (f->kpage == kpage)
        {
    f->pinned = pinned;
          break;
        }
    }
  
  FRAME_LOCK_RELEASE ();
}

/* Evict a frame using Second Chance Algorithm */
void *
frame_evict (void)
{
  struct frame *victim = NULL;
  struct thread *t = NULL;
  struct page *p = NULL;
  uint32_t *pd = NULL;
  bool dirty = false;
  size_t swap_slot;
  void *kpage = NULL;
  bool is_mmap = false;
  struct file *mmap_file = NULL;
  off_t mmap_offset = 0;
  bool needs_swap = false;

  FRAME_LOCK_ACQUIRE ();

  if (list_empty (&frame_table))
    {
      FRAME_LOCK_RELEASE ();
      return NULL;
    }

  if (clock_hand == NULL || clock_hand == list_end (&frame_table))
    clock_hand = list_begin (&frame_table);
  
  while (true)
    {
      if (clock_hand == list_end (&frame_table))
        clock_hand = list_begin (&frame_table);
      
      victim = list_entry (clock_hand, struct frame, elem);
      clock_hand = list_next (clock_hand);
      
      if (victim->pinned)
          continue;
      
      t = victim->thread;
      pd = t->pagedir;
      
      if (pagedir_is_accessed (pd, victim->upage))
        {
          pagedir_set_accessed (pd, victim->upage, false);
          continue;
        }
      
      /* Skip frames that are shared by multiple owners. */
      int ref_count = 0;
      struct list_elem *iter;
      for (iter = list_begin (&frame_table);
           iter != list_end (&frame_table);
           iter = list_next (iter))
        {
          struct frame *check = list_entry (iter, struct frame, elem);
          if (check->kpage == victim->kpage)
            ref_count++;
          if (ref_count > 1)
            break;
        }
      if (ref_count > 1)
        continue;

      break;
    }
  
  p = page_find (&t->spt, victim->upage);
  dirty = pagedir_is_dirty (pd, victim->upage);
  kpage = victim->kpage;

  if (p != NULL)
    {
      is_mmap = p->is_mmap;
      mmap_file = p->mmap_file;
      mmap_offset = p->mmap_offset;
      needs_swap = (dirty || p->location == PAGE_IN_MEMORY) && !is_mmap;
    }
  else
    {
      is_mmap = false;
      needs_swap = false;
    }

      pagedir_clear_page (pd, victim->upage);
      list_remove (&victim->elem);
  free (victim);

  FRAME_LOCK_RELEASE ();

  if (p == NULL)
    return kpage;

  if (is_mmap && dirty && mmap_file != NULL)
    {
      lock_acquire (&filesys_lock);
      file_write_at (mmap_file, kpage, PGSIZE, mmap_offset);
      lock_release (&filesys_lock);
      p->location = PAGE_IN_FILE;
    }
  else if (needs_swap)
    {
      swap_slot = swap_alloc ();
      if (swap_slot == SWAP_ERROR)
        PANIC ("swap space exhausted");
      swap_write (swap_slot, kpage);
      page_set_swap (p, swap_slot);
    }
  else
    {
      p->location = PAGE_IN_FILE;
    }

  return kpage;
}

