#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <stdbool.h>
#include <stdint.h>
#include <hash.h>
#include "filesys/file.h"
#include "threads/thread.h"

/* Supplemental Page Table Entry */
enum page_location
  {
    PAGE_IN_MEMORY,      /* Page is in physical memory */
    PAGE_IN_FILE,         /* Page is in file (lazy loading) */
    PAGE_IN_SWAP,         /* Page is in swap disk */
    PAGE_ZERO             /* Page is zero-initialized (stack) */
  };

struct page
  {
    void *vaddr;                    /* User virtual address */
    enum page_location location;    /* Where the page is located */
    
    /* For PAGE_IN_FILE */
    struct file *file;              /* File containing the page */
    off_t file_offset;              /* Offset in file */
    uint32_t read_bytes;            /* Bytes to read from file */
    uint32_t zero_bytes;            /* Bytes to zero */
    bool writable;                  /* Whether page is writable */
    
    /* For PAGE_IN_SWAP */
    size_t swap_slot;               /* Swap slot index */
    
    /* For mmap */
    struct file *mmap_file;         /* Mapped file (if mmap) */
    off_t mmap_offset;              /* Offset in mapped file */
    bool is_mmap;                   /* Whether this is an mmap page */
    
    struct hash_elem hash_elem;     /* Hash table element */
  };

/* Supplemental Page Table functions */
void page_init (struct hash *spt);
void page_destroy (struct hash *spt);
struct page *page_alloc (void *vaddr);
struct page *page_find (struct hash *spt, void *vaddr);
void page_free (struct hash *spt, struct page *p);
bool page_set_file (struct page *p, struct file *file, off_t offset,
                    uint32_t read_bytes, uint32_t zero_bytes, bool writable);
bool page_set_swap (struct page *p, size_t swap_slot);
bool page_set_zero (struct page *p);

/* Hash functions for SPT */
unsigned page_hash (const struct hash_elem *e, void *aux UNUSED);
bool page_less (const struct hash_elem *a, const struct hash_elem *b, void *aux UNUSED);

#endif /* vm/page.h */

