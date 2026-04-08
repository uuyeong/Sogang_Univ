#ifndef VM_SWAP_H
#define VM_SWAP_H

#include <stdbool.h>
#include <stddef.h>
#include "devices/block.h"

#define SWAP_ERROR SIZE_MAX

/* Swap table functions */
void swap_init (void);
size_t swap_alloc (void);
void swap_free (size_t slot);
void swap_read (size_t slot, void *kpage);
void swap_write (size_t slot, void *kpage);

#endif /* vm/swap.h */

