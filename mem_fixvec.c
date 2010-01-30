#include "mem_fixvec.h"

#include <string.h>

#include "except.h"
#include "mem.h"

#define DEFINE_FIXVEC_TYPE(N)						\
									\
    static size_t fv##N##_size_op(const heap_object_t *obj)		\
    { 									\
	return sizeof (fixvec##N##_t); 					\
    } 									\
 									\
    static size_t fv##N##_ptr_count_op(const heap_object_t *obj)	\
    { 									\
	return N; 							\
    } 									\
 									\
    static void fv##N##_move_op(const heap_object_t *src, heap_object_t *dst)\
    { 									\
	*(fixvec##N##_t *)dst = *(const fixvec##N##_t *)src;		\
    } 									\
 									\
    static obj_t fv##N##_get_ptr_op(const heap_object_t *obj, size_t index)\
    { 									\
	fixvec##N##_t *vec = (fixvec##N##_t *)obj; 			\
	ASSERT(index < N);						\
	return vec->fv##N##_ptrs[index]; 				\
    } 									\
 									\
    static void fv##N##_set_ptr_op(heap_object_t *obj,			\
				   size_t index,			\
				   obj_t ptr)				\
    { 									\
	fixvec##N##_t *vec = (fixvec##N##_t *)obj; 			\
	ASSERT(index < N);						\
	vec->fv##N##_ptrs[index] = ptr; 				\
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
    };

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
	super = NULL;
    }

    *ops = *super;
    ops->mo_name = name;
    ops->mo_super = super;
}

obj_t make_fixvec1(mem_ops_t *ops, obj_t ptr0)
{
    CHECK_OBJ(ptr0);
    heap_object_t *hobj = mem_alloc_obj(ops, sizeof (fixvec1_t));
    fixvec1_t *vec = (fixvec1_t *)hobj;
    vec->fv1_ptrs[0] = ptr0;
    return (obj_t)hobj;
}

obj_t make_fixvec2(mem_ops_t *ops, obj_t ptr0, obj_t ptr1)
{
    CHECK_OBJ(ptr0);
    CHECK_OBJ(ptr1);
    heap_object_t *hobj = mem_alloc_obj(ops, sizeof (fixvec2_t));
    fixvec2_t *vec = (fixvec2_t *)hobj;
    vec->fv2_ptrs[0] = ptr0;
    vec->fv2_ptrs[1] = ptr1;
    return (obj_t)hobj;
}
