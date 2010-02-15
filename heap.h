#ifndef HEAP_INCLUDED
#define HEAP_INCLUDED

#include <stddef.h>

#undef GROW_HEAP

#ifndef GROW_HEAP
/* set_heap_size_bytes must be called  before init_heap. */
extern void set_heap_size_bytes(size_t usable_size_bytes);
#endif

/* init_heap must be called before mem_alloc_obj. */
extern void init_heap(void);

extern void collect_garbage(void);

#endif /* !HEAP_INCLUDED */
