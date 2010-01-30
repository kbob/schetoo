#ifndef OBJ_CONT_INCLUDED
#define OBJ_CONT_INCLUDED

/* continuation object */

#include "mem_mixvec.h"

typedef obj_t (*cont_proc_t)(obj_t cont, obj_t *values);

extern obj_t make_continuation3(cont_proc_t proc, obj_t cont, obj_t arg);

static inline bool is_continuation(obj_t obj)
{
    extern mem_ops_t cont3_ops;
    return is_heap(obj) && *(mem_ops_t **)obj == &cont3_ops;
}

static inline cont_proc_t continuation_proc(obj_t cont)
{
    return (cont_proc_t)mixvec_1_2_get_int(cont, 0);
}

static inline obj_t continuation_cont(obj_t cont)
{
    return mixvec_1_2_get_ptr(cont, 0);
}

static inline obj_t continuation3_arg(obj_t cont)
{
    return mixvec_1_2_get_ptr(cont, 1);
}

#endif /* !OBJ_CONT_INCLUDED */
