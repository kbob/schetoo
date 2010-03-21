#ifndef MEM_MIXVEC_INCLUDED
#define MEM_MIXVEC_INCLUDED

#include <assert.h>

#include "except.h"
#include "mem.h"

/*
 * A mixvec is an abstract base type for objects that have a fixed
 * number of scalar fields and a fixed number of object pointers.
 * 
 * Example mixvecs: binding, procedure.
 */

extern void mem_mixvec_create_ops(mem_ops_t *,
				  wchar_t   *name,
				  size_t     word_count,
				  size_t     ptr_count);


#define DECLARE_MIXVEC(M, N)						\
    DECLARE_MIXVECMVMN_(M, N, M##_##N, mixvec_##M##_##N)

#define DECLARE_MIXVECMVMN_(M, N, MN, MVMN) 				\
									\
    typedef struct mixvec_##MN {					\
	heap_object_t mv_##MN##_header;					\
	word_t        mv_##MN##_ints[M];				\
	obj_t         mv_##MN##_ptrs[N];				\
    } mixvec_##MN##_t;							\
									\
    static inline obj_t alloc_##MVMN(mem_ops_t *ops)			\
    {									\
	return (obj_t) mem_alloc_obj(ops, sizeof (mixvec_##MN##_t));	\
    }									\
									\
    static inline word_t MVMN##_get_int(obj_t mv, size_t index)		\
    {									\
        assert(index < M);						\
	return ((mixvec_##MN##_t *)mv)->mv_##MN##_ints[index];		\
    }									\
									\
    static inline void MVMN##_set_int(obj_t mv,				\
				      size_t index,			\
				      intptr_t value)			\
    {									\
        assert(index < M);						\
	((mixvec_##MN##_t *)mv)->mv_##MN##_ints[index] = value;		\
    }									\
									\
    static inline obj_t MVMN##_get_ptr(obj_t mv, size_t index)		\
    {									\
	assert(index < N);						\
	return ((mixvec_##MN##_t *)mv)->mv_##MN##_ptrs[index];		\
    }									\
									\
    static inline void MVMN##_set_ptr(obj_t mv, size_t index, obj_t ptr)\
    {									\
	assert(index < N);						\
	((mixvec_##MN##_t *)mv)->mv_##MN##_ptrs[index] = ptr;		\
    }

DECLARE_MIXVEC(1, 2)
DECLARE_MIXVEC(1, 3)
DECLARE_MIXVEC(1, 4)
DECLARE_MIXVEC(1, 5)

extern obj_t make_mixvec_1_2(mem_ops_t *, word_t, obj_t, obj_t);
extern obj_t make_mixvec_1_3(mem_ops_t *, word_t, obj_t, obj_t, obj_t);
extern obj_t make_mixvec_1_4(mem_ops_t *, word_t, obj_t, obj_t, obj_t, obj_t);
extern obj_t make_mixvec_1_5(mem_ops_t *,
			     word_t, obj_t, obj_t, obj_t, obj_t, obj_t);

/* Declare more as needed. */

#endif /* !MEM_MIXVEC_INCLUDED */
