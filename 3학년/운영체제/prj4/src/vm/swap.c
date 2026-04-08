#include "vm/swap.h"
#include "devices/block.h"
#include "lib/kernel/bitmap.h"
#include "threads/synch.h"
#include "threads/vaddr.h"

/* Swap disk */
static struct block *swap_block;

/* Bitmap to track swap slots */
static struct bitmap *swap_bitmap;

/* Lock for swap operations */
static struct lock swap_lock;

/* Sectors per page */
#define SECTORS_PER_PAGE (PGSIZE / BLOCK_SECTOR_SIZE)

/* Initialize swap table */
void
swap_init (void)
{
  swap_block = block_get_role (BLOCK_SWAP);
  if (swap_block == NULL)
    return;
  
  /* Calculate number of swap slots */
  size_t swap_size = block_size (swap_block) / SECTORS_PER_PAGE;
  
  /* Create bitmap */
  swap_bitmap = bitmap_create (swap_size);
  if (swap_bitmap == NULL)
    return;
  
  lock_init (&swap_lock);
}

/* Allocate a swap slot */
size_t
swap_alloc (void)
{
  size_t slot;
  
  if (swap_block == NULL || swap_bitmap == NULL)
    return SWAP_ERROR;
  
  lock_acquire (&swap_lock);
  
  slot = bitmap_scan_and_flip (swap_bitmap, 0, 1, false);
  
  lock_release (&swap_lock);
  
  return slot != BITMAP_ERROR ? slot : SWAP_ERROR;
}

/* Free a swap slot */
void
swap_free (size_t slot)
{
  if (swap_block == NULL || swap_bitmap == NULL)
    return;
  
  if (slot == SWAP_ERROR)
    return;
  
  lock_acquire (&swap_lock);
  
  bitmap_set (swap_bitmap, slot, false);
  
  lock_release (&swap_lock);
}

/* Read a page from swap */
void
swap_read (size_t slot, void *kpage)
{
  size_t i;
  
  if (swap_block == NULL || slot == SWAP_ERROR)
    return;
  
  /* Acquire lock for synchronization */
  lock_acquire (&swap_lock);
  
  /* Read page from swap disk */
  for (i = 0; i < SECTORS_PER_PAGE; i++)
    {
      block_read (swap_block, slot * SECTORS_PER_PAGE + i,
                  (uint8_t *) kpage + i * BLOCK_SECTOR_SIZE);
    }
  
  lock_release (&swap_lock);
}

/* Write a page to swap */
void
swap_write (size_t slot, void *kpage)
{
  size_t i;
  
  if (swap_block == NULL || slot == SWAP_ERROR)
    return;
  
  /* Acquire lock for synchronization */
  lock_acquire (&swap_lock);
  
  /* Write page to swap disk */
  for (i = 0; i < SECTORS_PER_PAGE; i++)
    {
      block_write (swap_block, slot * SECTORS_PER_PAGE + i,
                   (uint8_t *) kpage + i * BLOCK_SECTOR_SIZE);
    }
  
  lock_release (&swap_lock);
}

