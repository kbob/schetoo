#include "mem.h"

#include <stdio.h>			/* XXX */
#include <stdlib.h>

#include "except.h"
#include "obj_eof.h"
#include "obj_fixnum.h"
#include "roots.h"

#define DEBUG_HEAP 0
#if DEBUG_HEAP
#include <stdio.h>
#endif /* DEBUG_HEAP */

#define INITIAL_HEAP_WORDS (1 << 18)
#define INITIAL_HEAP_BYTES (INITIAL_HEAP_WORDS * sizeof (word_t))

static void *the_heap;
static size_t heap_size_bytes = INITIAL_HEAP_BYTES;
static void *tospace;
static void *tospace_end;
static void *next_alloc;
static void *alloc_end;
static void *next_scan;
static void *fromspace, *fromspace_end;
static bool heap_is_initialized;

static inline bool is_in_tospace(const obj_t obj)
{
    const void *vobj = obj;
    return !is_normal(obj) || (vobj >= tospace && vobj < next_alloc);
}

static inline size_t aligned_size(size_t size)
{
    size_t oam1 = OBJ_ALIGN - 1;
    size_t mask = ~oam1;
    return (size + oam1) & mask;
}

#if DEBUG_HEAP

    #define IS_IN_FROMSPACE(ptr) \
	((void *)(ptr) >= fromspace && (void *)(ptr) < fromspace_end)

    static bool debug_heap = true;

    #define KNOWN_OPS_SIZE 20		/* increase as needed. */
    static const mem_ops_t *known_ops[KNOWN_OPS_SIZE];
    static size_t n_known_ops = 0;

    bool is_known_ops(const mem_ops_t *ops)
    {
	int i;
	for (i = 0; i < n_known_ops; i++)
	    if (ops == known_ops[i])
		return true;
	return false;
    }

    void remember_ops(const mem_ops_t *ops)
    {
	if (!is_known_ops(ops)) {
	    ASSERT(n_known_ops < KNOWN_OPS_SIZE - 1);
	    known_ops[n_known_ops++] = ops;
	}
    }

    static void verify_object(obj_t obj, bool scanned)
    {
	if (scanned)
	    ASSERT(!is_forward(obj));
	heap_object_t *hobj = obj_heap_object(obj);
	mem_ops_t *ops = obj_mem_ops(obj);
	ASSERT(is_known_ops(ops));
	size_t i, nptr = ops->mo_ptr_count(hobj);
	for (i = 0; i < nptr; i++) {
	    obj_t ptr = ops->mo_get_ptr(hobj, i);
	    if (scanned || !fromspace) {
		ASSERT(is_in_tospace(ptr));
		if (is_normal(ptr))
		    ASSERT(is_known_ops(obj_mem_ops(ptr)));
	    }
	    else {
		ASSERT(is_in_tospace(ptr) || IS_IN_FROMSPACE(ptr));
		if (!is_null(ptr))
		    ASSERT(is_forward(ptr) || is_known_ops(obj_mem_ops(ptr)));
	    }
	}
    }

    static void verify_heap()
    {
	if (!debug_heap)
	    return;
	void *p = tospace;
	while (p < next_scan) {
	    obj_t obj = (obj_t)p;
	    heap_object_t *hobj = obj_heap_object(p);
	    mem_ops_t *ops = heap_object_mem_ops(hobj);
	    size_t size = aligned_size(ops->mo_size(hobj));
	    verify_object(obj, true);
	    p += size;
	}
	if (p != next_scan) {
	    printf("verify_heap: to_space=%p\n", tospace);
	    printf("            next_scan=%p\n", next_scan);
	    printf("                    p=%p\n", p);
	    printf("           next_alloc=%p\n", next_alloc);
	    printf("         to_space_end=%p\n", tospace_end);
	    printf("            alloc_end=%p\n", alloc_end);
	}
	ASSERT(p == next_scan);
	while (p < next_alloc) {
	    obj_t obj = (obj_t)p;
	    heap_object_t *hobj = obj_heap_object(obj);
	    mem_ops_t *ops = heap_object_mem_ops(hobj);
	    ASSERT(is_known_ops(ops));
	    verify_object(obj, false);
	    size_t size = aligned_size(ops->mo_size(hobj));
	    size_t i, nptr = ops->mo_ptr_count(hobj);
	    for (i = 0; i < nptr; i++)
		ops->mo_get_ptr(hobj, i);
	    p += size;
	}
    }

#else

    static bool debug_heap = false;

    #define verify_heap()     ((void)0)
    #define remember_ops(ops) ((void)0)
    #define is_known_ops(ops) (true)

#endif

static void flip()
{
    fromspace = tospace;
    fromspace_end = tospace_end;
    if (tospace == the_heap) {
	tospace = the_heap + heap_size_bytes / 2;
	alloc_end = tospace_end = the_heap + heap_size_bytes;
    } else {
	tospace = the_heap;
	alloc_end = tospace_end = the_heap + heap_size_bytes / 2;
    }
    next_alloc = next_scan = tospace;
    if (debug_heap) {
	word_t *p;
	for (p = (word_t *)tospace; p < (word_t *)tospace_end; p++)
	    *p = 0xdeafb0bb;
    }
}

static obj_t move_obj(obj_t obj)
{
    if (is_fixnum(obj) || is_immediate(obj) || is_in_tospace(obj))
	return obj;
    if (is_forward(obj))
	return (obj_t)obj_fwd_ptr(obj);
    heap_object_t *hobj = obj_heap_object(obj);
    ASSERT(is_known_ops(obj_mem_ops(obj)));
    size_t size = aligned_size(obj_mem_ops(obj)->mo_size(hobj));
    ASSERT(next_alloc + size <= alloc_end);
    obj_t new_obj = next_alloc;
    next_alloc += size;
    heap_object_mem_ops(hobj)->mo_move(hobj, obj_heap_object(new_obj));
    heap_object_set_fwd_ptr(hobj, new_obj);
    return new_obj;
}

static void *scan_obj(heap_object_t *hobj)
{
    mem_ops_t *ops = heap_object_mem_ops(hobj);
    ASSERT(is_known_ops(ops));
    size_t size = aligned_size(ops->mo_size(hobj));
    size_t i, n_ptrs = ops->mo_ptr_count(hobj);
    for (i = 0; i < n_ptrs; i++) {
	ops->mo_set_ptr(hobj, i, move_obj(ops->mo_get_ptr(hobj, i)));
    }
    return (void *)hobj + size;
}

static void copy_heap()
{
    /* with lock */ {
	verify_heap();
	flip();
	verify_heap();
	root_descriptor_t *desc;
	for (desc = get_thread_roots(); desc; desc = desc->rd_next) {
	    *desc->rd_root = move_obj(*desc->rd_root);
	    verify_heap();
	}
	while (next_scan < next_alloc) {
	    next_scan = scan_obj(next_scan);
	    verify_heap();
	}
	ASSERT(next_scan == next_alloc);
	/* if (debug_heap)
	 *     alloc_end = next_alloc;
	 *else*/ if (alloc_end - next_alloc < (tospace_end - tospace) / 2)
		 fprintf(stderr, "increase heap size\n");
    }
}

void set_heap_size_bytes(size_t size_bytes)
{
    ASSERT(!heap_is_initialized);
    if (heap_size_bytes != size_bytes) {
	heap_size_bytes = size_bytes;
    }	
}

void init_heap(void)
{
    ASSERT(!heap_is_initialized);
    the_heap = malloc(heap_size_bytes);
    tospace = the_heap;
    alloc_end = tospace_end = the_heap + heap_size_bytes / 2;
    next_alloc = the_heap;
    next_scan = the_heap;
    // if (debug_heap)
    //     alloc_end = next_alloc;

    heap_is_initialized = true;
}

heap_object_t *mem_alloc_obj(mem_ops_t *ops, size_t size_bytes)
{
    ASSERT(heap_is_initialized);
    verify_heap();
    remember_ops(ops);
    size_t alloc_size = aligned_size(size_bytes);
    if (next_alloc > alloc_end - alloc_size) {
	ASSERT(false && "implement longjmp to GC");
	copy_heap();
	ASSERT(next_alloc <= tospace_end - alloc_size && "out of memory");
    }
    heap_object_t *p;
    /* with lock */ {
        p = next_alloc;
        next_alloc += alloc_size;
    }
    p->oh_ops = ops;
    return p;
}

const wchar_t *object_type_name(const obj_t obj)
{
    if (is_fixnum(obj))
	return L"fixnum";
    if (is_forward(obj))
	return L"forward";
    if (is_null(obj))
        return L"null";
    if (is_undefined(obj))
	return L"undefined";
    if (is_eof(obj))
	return L"eof-object";
    if (is_special(obj))
	return L"special";
    return obj_mem_ops(obj)->mo_name;
}

#ifndef NDEBUG
void check_obj(const obj_t obj)
{
    ASSERT(heap_is_initialized);
    ASSERT(is_fixnum(obj) || is_immediate(obj) || is_in_tospace(obj));
}
#endif /* !NDEBUG */
