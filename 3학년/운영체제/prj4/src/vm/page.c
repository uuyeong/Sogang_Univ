#include "vm/page.h"
#include "vm/frame.h"
#include "vm/swap.h"
#include "threads/malloc.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "filesys/file.h"

/* Hash function for supplemental page table */
unsigned
page_hash (const struct hash_elem *e, void *aux UNUSED)
{
  const struct page *p = hash_entry (e, struct page, hash_elem);
  return hash_int ((int) p->vaddr);
}

/* Comparison function for supplemental page table */
bool
page_less (const struct hash_elem *a, const struct hash_elem *b, void *aux UNUSED)
{
  const struct page *pa = hash_entry (a, struct page, hash_elem);
  const struct page *pb = hash_entry (b, struct page, hash_elem);
  return pa->vaddr < pb->vaddr;
}

/* Initialize supplemental page table */
void
page_init (struct hash *spt)
{
  hash_init (spt, page_hash, page_less, NULL);
}

/* Free a page entry (helper for hash_destroy) */
static void
page_free_entry (struct hash_elem *e, void *aux UNUSED)
{
  struct page *p = hash_entry (e, struct page, hash_elem);
  
  if (p->location == PAGE_IN_SWAP)
    {
      swap_free (p->swap_slot);
    }
  
  free (p);
}

/* Destroy supplemental page table */
void
page_destroy (struct hash *spt)
{
  hash_destroy (spt, page_free_entry);
}

/* Allocate a new page entry */
struct page *
page_alloc (void *vaddr)
{
  struct page *p = malloc (sizeof (struct page));
  if (p == NULL)
    return NULL;
  
  p->vaddr = pg_round_down (vaddr);
  p->location = PAGE_ZERO;
  p->file = NULL;
  p->file_offset = 0;
  p->read_bytes = 0;
  p->zero_bytes = 0;
  p->writable = false;
  p->swap_slot = 0;
  p->mmap_file = NULL;
  p->mmap_offset = 0;
  p->is_mmap = false;
  
  return p;
}

/* Find a page entry in SPT */
struct page *
page_find (struct hash *spt, void *vaddr)
{
  struct page p;
  struct hash_elem *e;
  
  p.vaddr = pg_round_down (vaddr);
  e = hash_find (spt, &p.hash_elem);
  
  return e != NULL ? hash_entry (e, struct page, hash_elem) : NULL;
}

/* Free a page entry from SPT */
void
page_free (struct hash *spt, struct page *p)
{
  if (p != NULL)
    {
      hash_delete (spt, &p->hash_elem);
      if (p->location == PAGE_IN_SWAP)
        {
          swap_free (p->swap_slot);
        }
      free (p);
    }
}

/* Set page to be loaded from file */
bool
page_set_file (struct page *p, struct file *file, off_t offset,
               uint32_t read_bytes, uint32_t zero_bytes, bool writable)
{
  if (p == NULL)
    return false;
  
  p->location = PAGE_IN_FILE;
  p->file = file;
  p->file_offset = offset;
  p->read_bytes = read_bytes;
  p->zero_bytes = zero_bytes;
  p->writable = writable;
  
  return true;
}

/* Set page to be in swap */
bool
page_set_swap (struct page *p, size_t swap_slot)
{
  if (p == NULL)
    return false;
  
  p->location = PAGE_IN_SWAP;
  p->swap_slot = swap_slot;
  
  return true;
}

/* Set page to be zero-initialized */
bool
page_set_zero (struct page *p)
{
  if (p == NULL)
    return false;
  
  p->location = PAGE_ZERO;
  
  return true;
}

