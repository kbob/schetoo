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

extern obj_t make_cont3(cont_proc_t proc, obj_t cont, obj_t env);
extern obj_t make_cont4(cont_proc_t proc, obj_t cont,
			obj_t env, obj_t arg);

extern obj_t make_cont5(cont_proc_t proc, obj_t cont,
			obj_t env, obj_t arg1, obj_t arg2);
extern obj_t make_cont6(cont_proc_t proc, obj_t cont,
			obj_t env, obj_t arg1, obj_t arg2, obj_t arg3);

static inline bool        is_cont   (obj_t);
static inline bool        is_cont3  (obj_t);
static inline bool        is_cont4  (obj_t);
static inline bool        is_cont5  (obj_t);
static inline bool        is_cont6  (obj_t);
static inline cont_proc_t cont_proc (obj_t cont);
static inline obj_t       cont_cont (obj_t cont);
static inline obj_t       cont_env  (obj_t cont);
static inline obj_t       cont4_arg (obj_t cont);
static inline obj_t       cont5_arg1(obj_t cont);
static inline obj_t       cont5_arg2(obj_t cont);
static inline obj_t       cont6_arg1(obj_t cont);
static inline obj_t       cont6_arg2(obj_t cont);
static inline obj_t       cont6_arg3(obj_t cont);

OBJ_TYPE_PREDICATE(cont3);		// bool is_cont3(obj_t);
OBJ_TYPE_PREDICATE(cont4);		// bool is_cont4(obj_t);
OBJ_TYPE_PREDICATE(cont5);		// bool is_cont5(obj_t);
OBJ_TYPE_PREDICATE(cont6);		// bool is_cont6(obj_t);

static inline bool is_cont(obj_t obj)
{
    return is_cont3(obj) ||  is_cont4(obj) || is_cont5(obj) || is_cont6(obj);
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

static inline obj_t cont5_arg1(obj_t cont)
{
    CHECK(is_cont5(cont), "must be cont5", cont);
    return mixvec_1_4_get_ptr(cont, 2);
}

static inline obj_t cont5_arg2(obj_t cont)
{
    CHECK(is_cont5(cont), "must be cont5", cont);
    return mixvec_1_4_get_ptr(cont, 3);
}

static inline obj_t cont6_arg1(obj_t cont)
{
    CHECK(is_cont6(cont), "must be cont6", cont);
    return mixvec_1_5_get_ptr(cont, 2);
}

static inline obj_t cont6_arg2(obj_t cont)
{
    CHECK(is_cont6(cont), "must be cont6", cont);
    return mixvec_1_5_get_ptr(cont, 3);
}

static inline obj_t cont6_arg3(obj_t cont)
{
    CHECK(is_cont6(cont), "must be cont6", cont);
    return mixvec_1_5_get_ptr(cont, 4);
}

#endif /* !OBJ_CONT_INCLUDED */
