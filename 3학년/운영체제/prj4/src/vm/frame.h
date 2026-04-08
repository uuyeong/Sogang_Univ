#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <stdbool.h>
#include <stdint.h>
#include <list.h>
#include "threads/palloc.h"
#include "threads/synch.h"
#include "filesys/off_t.h"

struct file;

/* Frame table entry */
struct frame
  {
    void *kpage;                    /* Kernel virtual address (physical frame) */
    void *upage;                    /* User virtual address */
    struct thread *thread;          /* Thread that owns this frame */
    struct list_elem elem;          /* List element for frame table */
    bool pinned;                    /* Whether frame is pinned (cannot be evicted) */
    
    /* For page merging - file and offset information */
    struct file *file;              /* File containing the page (if file-backed) */
    off_t file_offset;              /* Offset in file */
    bool read_only;                 /* Whether page is read-only (for executable code sharing) */
  };

/* Frame table functions */
void frame_init (void);
void *frame_alloc (enum palloc_flags flags, void *upage);
void *frame_alloc_with_file (enum palloc_flags flags, void *upage, struct file *file, off_t offset);
void frame_free (void *kpage);
void frame_set_pinned (void *kpage, bool pinned);
struct frame *frame_find (void *kpage);
void *frame_find_shared (struct file *file, off_t offset);
bool frame_add_entry (void *kpage, void *upage);
void *frame_find_and_add_shared (struct file *file, off_t offset, void *upage, bool *found);

/* Eviction functions */
void *frame_evict (void);

#endif /* vm/frame.h */

