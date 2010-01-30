#include "mem_mixvec.h"

#include "except.h"
#include "mem.h"

#define DEFINE_MIXVEC_TYPE(M, N, MN)					\
									\
    static size_t mv_##MN##_size_op(const heap_object_t *obj)		\
    {									\
	return sizeof (mixvec_##MN##_t);				\
    }									\
									\
    static size_t mv_##MN##_ptr_count_op(const heap_object_t *obj)	\
    {									\
	return N;							\
    }									\
									\
    static void mv_##MN##_move_op(const heap_object_t *src,		\
				  heap_object_t *dst)			\
    {									\
	*(mixvec_##MN##_t *)dst = *(const mixvec_##MN##_t *)src;	\
    }									\
									\
    static obj_t mv_##MN##_get_ptr_op(const heap_object_t *obj,		\
				      size_t index)			\
    {									\
	ASSERT(index < N);						\
	return ((mixvec_##MN##_t *)obj)->mv_##MN##_ptrs[index];		\
    }									\
									\
    static void mv_##MN##_set_ptr_op(heap_object_t *obj,		\
				     size_t index,			\
				     obj_t ptr)				\
    {									\
	ASSERT(index < N);						\
	((mixvec_##MN##_t *)obj)->mv_##MN##_ptrs[index] = ptr;		\
    }									\
									\
    static mem_ops_t mixvec_##MN##_ops = {				\
	MEM_OPS_PRIMITIVE,						\
	L"mixvec_" #MN,							\
	NULL,								\
	mv_##MN##_size_op,						\
	mv_##MN##_ptr_count_op,						\
	mv_##MN##_move_op,						\
	mv_##MN##_get_ptr_op,						\
	mv_##MN##_set_ptr_op,						\
	{ }								\
    };

DEFINE_MIXVEC_TYPE(1, 2, 1_2)
DEFINE_MIXVEC_TYPE(1, 3, 1_3)


extern obj_t make_mixvec_1_2(mem_ops_t *ops, word_t w0, obj_t p0, obj_t p1)
{
    obj_t mv = alloc_mixvec_1_2(ops);
    mixvec_1_2_set_int(mv, 0, w0);
    mixvec_1_2_set_ptr(mv, 0, p0);
    mixvec_1_2_set_ptr(mv, 1, p1);
    return mv;
}

extern obj_t make_mixvec_1_3(mem_ops_t *ops,
			     word_t w0,
			     obj_t p0, obj_t p1, obj_t p2)
{
    obj_t mv = alloc_mixvec_1_3(ops);
    mixvec_1_3_set_int(mv, 0, w0);
    mixvec_1_3_set_ptr(mv, 0, p0);
    mixvec_1_3_set_ptr(mv, 1, p1);
    mixvec_1_3_set_ptr(mv, 2, p2);
    return mv;
}

void mem_mixvec_create_ops(mem_ops_t  *ops,
			   wchar_t    *name,
			   size_t      nints,
			   size_t      nptrs)
{
    mem_ops_t *super;
    if (nints == 1 && nptrs == 2)
	super = &mixvec_1_2_ops;
    else if (nints == 1 && nptrs == 3)
	super = &mixvec_1_3_ops;
    else
	ASSERT(false);
    *ops = *super;
    ops->mo_name = name;
    ops->mo_super = super;
}
