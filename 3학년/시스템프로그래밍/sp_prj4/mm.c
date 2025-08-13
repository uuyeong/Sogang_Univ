/*
 * mm.c - Explicit free lists with quick lists optimization
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mm.h"
#include "memlib.h"

team_t team = {"20231508", "강유영", "20231508@sogang.ac.kr"};

#define WSIZE       4       /* word size */
#define DSIZE       8       /* double word size */
#define CHUNKSIZE  (1<<12)  /* heap extension size */
#define LISTS       16      /* number of segregated lists */
#define QL_CLASSES  16      /* quick list classes */
#define QL_MAXSIZE  (QL_CLASSES * DSIZE)  /* max quick-list block size */

#define MAX(x,y)       ((x) > (y) ? (x) : (y))
#define PACK(size,alloc)  ((size) | (alloc))
#define GET(p)          (*(unsigned int *)(p))
#define PUT(p,val)      (*(unsigned int *)(p) = (val))
#define GET_SIZE(p)     (GET(p) & ~0x7)
#define GET_ALLOC(p)    (GET(p) & 0x1)

#define HDRP(bp)        ((char*)(bp) - WSIZE)
#define FTRP(bp)        ((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
#define NEXT_BLKP(bp)   ((char*)(bp) + GET_SIZE(((char*)(bp) - WSIZE)))
#define PREV_BLKP(bp)   ((char*)(bp) - GET_SIZE(((char*)(bp) - DSIZE)))

#define NEXT_FREE(bp)   (*(char **)(bp))
#define PREV_FREE(bp)   (*(char **)((char*)(bp) + sizeof(void *)))

static void *quick_lists[QL_CLASSES];
static void *seg_list[LISTS];

static size_t adjust_size(size_t size);
static int select_list(size_t asize);
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
static void insert_block(void *bp);
static void remove_block(void *bp);

int mm_init(void) {
    for (int i = 0; i < QL_CLASSES; i++)
        quick_lists[i] = NULL;
    for (int i = 0; i < LISTS; i++)
        seg_list[i] = NULL;
    char *heap = mem_sbrk(4*WSIZE);
    if (heap == (void*)-1) return -1;
    PUT(heap, 0);
    PUT(heap+WSIZE, PACK(DSIZE,1));
    PUT(heap+2*WSIZE, PACK(DSIZE,1));
    PUT(heap+3*WSIZE, PACK(0,1));
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) return -1;
    return 0;
}

void *mm_malloc(size_t size) {
    if (size == 0) return NULL;
    size_t asize = adjust_size(size);
    if (asize <= QL_MAXSIZE) {
        int qi = (asize/DSIZE) - 1;
        void *bp = quick_lists[qi];
        if (bp) {
            quick_lists[qi] = NEXT_FREE(bp);
            PUT(HDRP(bp), PACK(asize,1));
            PUT(FTRP(bp), PACK(asize,1));
            return bp;
        }
    }
    void *bp = find_fit(asize);
    if (bp) {
        place(bp, asize);
        return bp;
    }
    size_t extendsize = MAX(asize, CHUNKSIZE);
    bp = extend_heap(extendsize/WSIZE);
    if (!bp) return NULL;
    place(bp, asize);
    return bp;
}

void mm_free(void *bp) {
    if (!bp) return;
    size_t sz = GET_SIZE(HDRP(bp));
    if (sz <= QL_MAXSIZE) {
        int qi = (sz/DSIZE) - 1;
        NEXT_FREE(bp) = quick_lists[qi];
        quick_lists[qi] = bp;
        return;
    }
    PUT(HDRP(bp), PACK(sz,0));
    PUT(FTRP(bp), PACK(sz,0));
    coalesce(bp);
}

void *mm_realloc(void *ptr, size_t size) {
    if (!ptr) return mm_malloc(size);
    if (size == 0) { mm_free(ptr); return NULL; }
    void *newp = mm_malloc(size);
    if (!newp) return NULL;
    size_t copy = GET_SIZE(HDRP(ptr)) - DSIZE;
    if (size < copy) copy = size;
    memcpy(newp, ptr, copy);
    mm_free(ptr);
    return newp;
}

static size_t adjust_size(size_t size) {
    size_t asize = size + DSIZE;
    if (asize <= DSIZE)
        asize = 2*DSIZE;
    else
        asize = ((asize + (DSIZE-1)) / DSIZE) * DSIZE;
    return asize;
}

static int select_list(size_t asize) {
    int idx = 0;
    size_t s = DSIZE;
    while (idx < LISTS-1 && asize > s) {
        idx++; s <<= 1;
    }
    return idx;
}

static void *extend_heap(size_t words) {
    size_t sz = (words%2) ? (words+1)*WSIZE : words*WSIZE;
    char *bp = mem_sbrk(sz);
    if (bp == (void*)-1) return NULL;
    PUT(HDRP(bp), PACK(sz,0));
    PUT(FTRP(bp), PACK(sz,0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0,1));
    return coalesce(bp);
}

static void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(HDRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t sz = GET_SIZE(HDRP(bp));
    if (prev_alloc && next_alloc) {
    } else if (prev_alloc && !next_alloc) {
        void *n = NEXT_BLKP(bp);
        remove_block(n);
        sz += GET_SIZE(HDRP(n));
        PUT(HDRP(bp), PACK(sz,0));
        PUT(FTRP(bp), PACK(sz,0));
    } else if (!prev_alloc && next_alloc) {
        void *p = PREV_BLKP(bp);
        remove_block(p);
        sz += GET_SIZE(HDRP(p));
        bp = p;
        PUT(HDRP(bp), PACK(sz,0));
        PUT(FTRP(bp), PACK(sz,0));
    } else {
        void *p = PREV_BLKP(bp);
        void *n = NEXT_BLKP(bp);
        remove_block(p);
        remove_block(n);
        sz += GET_SIZE(HDRP(p)) + GET_SIZE(HDRP(n));
        bp = p;
        PUT(HDRP(bp), PACK(sz,0));
        PUT(FTRP(bp), PACK(sz,0));
    }
    insert_block(bp);
    return bp;
}

static void *find_fit(size_t asize) {
    int idx = select_list(asize);
    for (int i = idx; i < LISTS; i++) {
        void *bp = seg_list[i];
        while (bp) {
            if (asize <= GET_SIZE(HDRP(bp))) return bp;
            bp = NEXT_FREE(bp);
        }
    }
    return NULL;
}

static void place(void *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));
    remove_block(bp);
    if (csize - asize >= 2*DSIZE) {
        PUT(HDRP(bp), PACK(asize,1));
        PUT(FTRP(bp), PACK(asize,1));
        void *n = NEXT_BLKP(bp);
        PUT(HDRP(n), PACK(csize-asize,0));
        PUT(FTRP(n), PACK(csize-asize,0));
        insert_block(n);
    } else {
        PUT(HDRP(bp), PACK(csize,1));
        PUT(FTRP(bp), PACK(csize,1));
    }
}

static void remove_block(void *bp) {
    int idx = select_list(GET_SIZE(HDRP(bp)));
    if (PREV_FREE(bp))
        NEXT_FREE(PREV_FREE(bp)) = NEXT_FREE(bp);
    else
        seg_list[idx] = NEXT_FREE(bp);
    if (NEXT_FREE(bp))
        PREV_FREE(NEXT_FREE(bp)) = PREV_FREE(bp);
}

static void insert_block(void *bp) {
    int idx = select_list(GET_SIZE(HDRP(bp)));
    NEXT_FREE(bp) = seg_list[idx];
    PREV_FREE(bp) = NULL;
    if (seg_list[idx])
        PREV_FREE(seg_list[idx]) = bp;
    seg_list[idx] = bp;
}
