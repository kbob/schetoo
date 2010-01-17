#ifndef MEM_FIXVEC_INCLUDED
#define MEM_FIXVEC_INCLUDED

#include "mem.h"

/*
 * A fixvec is an abstract base type for fixed-length vectors of
 * object pointers.
 * 
 * Example fixvecs: symbol, pair.
 */

extern void mem_fixvec_create_ops(mem_ops_t *, wchar_t *name, size_t len);

extern obj_t alloc_fixvec1(mem_ops_t *, obj_t);
extern obj_t fixvec1_get_ptr(obj_t obj, size_t index);
extern void  fixvec1_set_ptr(obj_t obj, size_t index, obj_t ptr);

extern obj_t alloc_fixvec2(mem_ops_t *, obj_t, obj_t);
extern obj_t fixvec2_get_ptr(obj_t obj, size_t index);
extern void  fixvec2_set_ptr(obj_t obj, size_t index, obj_t ptr);

/* Add more as needed... */

#endif /* !MEM_FIXVEC_INCLUDED */
