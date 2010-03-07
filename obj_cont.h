#ifndef OBJ_CONT_INCLUDED
#define OBJ_CONT_INCLUDED

/* cont - continuation object */

#include "cv.h"
#include "mem_mixvec.h"

/*
 * Eventually, there may be multiple sizes of continuations, with
 * different numbers of saved values.  The functions below with prefix
 * "cont_" operate on any continuation.  The ones prefixed "contN_"
 * only operate on a specific continuation size.
 */

extern obj_t make_cont4(cont_proc_t proc, obj_t cont,
			obj_t arg1, obj_t arg2);

OBJ_TYPE_PREDICATE(cont4);		// bool is_cont4(obj_t);

static inline bool is_cont(obj_t obj)
{
    return is_cont4(obj);
}

static inline cont_proc_t cont_proc(obj_t cont)
{
    CHECK(is_cont(cont), "must be continuation", cont);
    return (cont_proc_t)mixvec_1_3_get_int(cont, 0);
}

static inline obj_t cont_cont(obj_t cont)
{
    CHECK(is_cont(cont), "must be continuation", cont);
    return mixvec_1_3_get_ptr(cont, 0);
}

static inline obj_t cont_env(obj_t cont)
{
    CHECK(is_cont(cont), "must be cont", cont);
    return mixvec_1_3_get_ptr(cont, 1);
}

static inline obj_t cont4_arg(obj_t cont)
{
    CHECK(is_cont4(cont), "must be cont4", cont);
    return mixvec_1_3_get_ptr(cont, 2);
}

#endif /* !OBJ_CONT_INCLUDED */
