#ifndef OBJ_CONT_INCLUDED
#define OBJ_CONT_INCLUDED

/* continuation object */

#include "mem_mixvec.h"

typedef obj_t (*cont_proc_t)(obj_t cont, obj_t *p_values, obj_t *p_env);

extern obj_t make_continuation3(cont_proc_t proc, obj_t cont, obj_t arg);
extern obj_t make_continuation4(cont_proc_t proc, obj_t cont,
				obj_t arg1, obj_t arg2);

OBJ_TYPE_PREDICATE(continuation3);	// bool is_continuation3(obj_t);
OBJ_TYPE_PREDICATE(continuation4);	// bool is_continuation3(obj_t);

static inline bool is_continuation(obj_t obj)
{
    return is_continuation3(obj) || is_continuation4(obj);
}

static inline cont_proc_t continuation_proc(obj_t cont)
{
    CHECK(is_continuation(cont), NULL, "must be continuation", cont);
    return (cont_proc_t)mixvec_1_2_get_int(cont, 0);
}

static inline obj_t continuation_cont(obj_t cont)
{
    CHECK(is_continuation(cont), NULL, "must be continuation", cont);
    return mixvec_1_2_get_ptr(cont, 0);
}

static inline obj_t continuation3_arg(obj_t cont)
{
    CHECK(is_continuation3(cont), NULL, "must be continuation3", cont);
    return mixvec_1_2_get_ptr(cont, 1);
}

static inline obj_t continuation4_arg1(obj_t cont)
{
    CHECK(is_continuation4(cont), NULL, "must be continuation4", cont);
    return mixvec_1_3_get_ptr(cont, 1);
}

static inline obj_t continuation4_arg2(obj_t cont)
{
    CHECK(is_continuation4(cont), NULL, "must be continuation4", cont);
    return mixvec_1_3_get_ptr(cont, 2);
}

#endif /* !OBJ_CONT_INCLUDED */
