#ifndef OBJ_CONT_INCLUDED
#define OBJ_CONT_INCLUDED

/* cont - continuation object */

#include "mem_mixvec.h"

typedef obj_t (*cont_proc_t)(obj_t cont, obj_t *p_values, obj_t *p_env);

extern obj_t make_cont3(cont_proc_t proc, obj_t cont, obj_t arg);
extern obj_t make_cont4(cont_proc_t proc, obj_t cont,
				obj_t arg1, obj_t arg2);

OBJ_TYPE_PREDICATE(cont3);		// bool is_cont3(obj_t);
OBJ_TYPE_PREDICATE(cont4);		// bool is_cont3(obj_t);

static inline bool is_cont(obj_t obj)
{
    return is_cont3(obj) || is_cont4(obj);
}

static inline cont_proc_t cont_proc(obj_t cont)
{
    CHECK(is_cont(cont), NULL, "must be continuation", cont);
    return (cont_proc_t)mixvec_1_2_get_int(cont, 0);
}

static inline obj_t cont_cont(obj_t cont)
{
    CHECK(is_cont(cont), NULL, "must be continuation", cont);
    return mixvec_1_2_get_ptr(cont, 0);
}

static inline obj_t cont3_arg(obj_t cont)
{
    CHECK(is_cont3(cont), NULL, "must be cont3", cont);
    return mixvec_1_2_get_ptr(cont, 1);
}

static inline obj_t cont4_arg1(obj_t cont)
{
    CHECK(is_cont4(cont), NULL, "must be cont4", cont);
    return mixvec_1_3_get_ptr(cont, 1);
}

static inline obj_t cont4_arg2(obj_t cont)
{
    CHECK(is_cont4(cont), NULL, "must be cont4", cont);
    return mixvec_1_3_get_ptr(cont, 2);
}

#endif /* !OBJ_CONT_INCLUDED */
