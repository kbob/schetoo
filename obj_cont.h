#ifndef OBJ_CONT_INCLUDED
#define OBJ_CONT_INCLUDED

/* cont - continuation object */

#include "mem_mixvec.h"

/*
 * A continuation bundles some values together with a continuation
 * proc.  A continuation proc returns two values: a continuation and a
 * list of evaluated expressions.  Since C doesn't have multivalued
 * functions, continuation procs are defined as returning a cv_t
 * (continuation + values).
 */

typedef struct cv {
    obj_t cv_cont;
    obj_t cv_values;
} cv_t;
typedef cv_t (*cont_proc_t)(obj_t cont, obj_t values);

/* cv() - cv_t constructor. */

static inline cv_t cv(obj_t cont, obj_t values)
{
    cv_t tmp = { cont, values };
    return tmp;
}

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
    CHECK(is_cont(cont), NULL, "must be continuation", cont);
    return (cont_proc_t)mixvec_1_3_get_int(cont, 0);
}

static inline obj_t cont_cont(obj_t cont)
{
    CHECK(is_cont(cont), NULL, "must be continuation", cont);
    return mixvec_1_3_get_ptr(cont, 0);
}

static inline obj_t cont_env(obj_t cont)
{
    CHECK(is_cont(cont), NULL, "must be cont", cont);
    return mixvec_1_3_get_ptr(cont, 1);
}

static inline obj_t cont4_arg(obj_t cont)
{
    CHECK(is_cont4(cont), NULL, "must be cont4", cont);
    return mixvec_1_3_get_ptr(cont, 2);
}

#endif /* !OBJ_CONT_INCLUDED */
