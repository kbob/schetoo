#ifndef MEM_FIXVEC_INCLUDED
#define MEM_FIXVEC_INCLUDED

#include "except.h"
#include "mem.h"

/*
 * A fixvec is an abstract base type for fixed-length vectors of
 * object pointers.
 * 
 * Example fixvecs: symbol, pair.
 */

#define DECLARE_FIXVEC_TYPE(N)						\
									\
    typedef struct fixvec##N { 						\
        heap_object_t fv##N##_header; 					\
        obj_t         fv##N##_ptrs[N]; 					\
    } fixvec##N##_t;							\
									\
    static inline obj_t alloc_fixvec##N(mem_ops_t *ops)			\
    {									\
        return (obj_t)mem_alloc_obj(ops, sizeof (fixvec1_t));		\
    }									\
									\
    static inline obj_t fixvec##N##_get_ptr(obj_t obj, size_t index)	\
    {									\
	CHECK_OBJ(obj);							\
	ASSERT(index < N);						\
	return ((fixvec##N##_t *)obj)->fv##N##_ptrs[index];		\
    }									\
									\
    static inline void							\
    fixvec##N##_set_ptr(obj_t obj, size_t index, obj_t ptr)		\
    {									\
	CHECK_OBJ(obj);							\
	ASSERT(index < N);						\
	((fixvec##N##_t *)obj)->fv##N##_ptrs[index] = ptr;		\
    }

extern void mem_fixvec_create_ops(mem_ops_t *, wchar_t *name, size_t len);

DECLARE_FIXVEC_TYPE(1);
extern obj_t make_fixvec1       (mem_ops_t *, obj_t);

DECLARE_FIXVEC_TYPE(2);
extern obj_t make_fixvec2       (mem_ops_t *, obj_t, obj_t);

/* Add more as needed... */

#endif /* !MEM_FIXVEC_INCLUDED */
