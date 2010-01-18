#ifndef HEAP_INCLUDED
#define HEAP_INCLUDED

#include <stddef.h>

/* set_heap_size_bytes must be called  before init_heap. */
extern void set_heap_size_bytes(size_t usable_size_bytes);

/* init_heap must be called before mem_alloc_obj. */
extern void init_heap(void);

#endif /* !HEAP_INCLUDED */
