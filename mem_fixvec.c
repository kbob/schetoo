#include "mem_fixvec.h"

#include <string.h>

#include "except.h"
#include "mem.h"

#define DEFINE_FIXVEC_TYPE(N)						\
									\
    typedef struct fixvec##N { 						\
        obj_header_t fv##N##_header; 					\
        obj_t        fv##N##_ptrs[N]; 					\
    } fixvec##N##_t;							\
									\
    static size_t fv##N##_size_op(const obj_header_t *obj)		\
    { 									\
	return sizeof (fixvec##N##_t); 					\
    } 									\
 									\
    static size_t fv##N##_ptr_count_op(const obj_header_t *obj)		\
    { 									\
	return N; 							\
    } 									\
 									\
    static void fv##N##_move_op(const obj_header_t *src, obj_header_t *dst)\
    { 									\
	*(fixvec##N##_t *)dst = *(const fixvec##N##_t *)src;		\
    } 									\
 									\
    static obj_t fv##N##_get_ptr_op(const obj_header_t *obj, size_t index)\
    { 									\
	fixvec##N##_t *vec = (fixvec##N##_t *)obj; 			\
	if (index < N) 							\
	    return vec->fv##N##_ptrs[index]; 				\
	ASSERT(false); 							\
    } 									\
 									\
    static void fv##N##_set_ptr_op(obj_header_t *obj, size_t index, obj_t ptr)	\
    { 									\
	fixvec##N##_t *vec = (fixvec##N##_t *)obj; 			\
	if (index < N) 							\
	    vec->fv##N##_ptrs[index] = ptr; 				\
	else 								\
	    ASSERT(false); 						\
    } 									\
 									\
    static mem_ops_t fixvec##N##_ops = { 				\
	MEM_OPS_PRIMITIVE,						\
	L"fixvec" #N, 							\
	NULL, 								\
	fv##N##_size_op, 						\
	fv##N##_ptr_count_op, 						\
	fv##N##_move_op, 						\
	fv##N##_get_ptr_op, 						\
	fv##N##_set_ptr_op, 						\
	{ } 								\
    };									\
									\
    obj_t fixvec##N##_get_ptr(obj_t obj, size_t index)			\
    {									\
	check_obj(obj);						\
	ASSERT(index < N);						\
	return ((fixvec##N##_t *)obj)->fv##N##_ptrs[index];		\
    }									\
									\
    void fixvec##N##_set_ptr(obj_t obj, size_t index, obj_t ptr)	\
    {									\
	check_obj(obj);							\
	ASSERT(index < N);						\
	((fixvec##N##_t *)obj)->fv##N##_ptrs[index] = ptr;		\
    }

DEFINE_FIXVEC_TYPE(1)
DEFINE_FIXVEC_TYPE(2)

void mem_fixvec_create_ops(mem_ops_t *ops, wchar_t *name, size_t len)
{
    mem_ops_t *super;

    switch (len) {
    case 1:
	super = &fixvec1_ops;
	break;

    case 2:
	super = &fixvec2_ops;
	break;

    default:
	ASSERT(false);
    }

    *ops = *super;
    ops->mo_name = name;
    ops->mo_super = super;
}

obj_t alloc_fixvec1(mem_ops_t *ops, obj_t ptr0)
{
    check_obj(ptr0);
    obj_header_t *hdr = mem_alloc_obj(ops, sizeof (fixvec1_t));
    fixvec1_t *vec = (fixvec1_t *)hdr;
    vec->fv1_ptrs[0] = ptr0;
    return (obj_t)hdr;
}

obj_t alloc_fixvec2(mem_ops_t *ops, obj_t ptr0, obj_t ptr1)
{
    check_obj(ptr0);
    check_obj(ptr1);
    obj_header_t *hdr = mem_alloc_obj(ops, sizeof (fixvec2_t));
    fixvec2_t *vec = (fixvec2_t *)hdr;
    vec->fv2_ptrs[0] = ptr0;
    vec->fv2_ptrs[1] = ptr1;
    return (obj_t)hdr;
}
