#include "obj_cont.h"

#include "mem_mixvec.h"

mem_ops_t continuation3_ops;
mem_ops_t continuation4_ops;		/* XXX not used yet */

obj_t make_continuation3(cont_proc_t proc, obj_t cont, obj_t arg)
{
    CHECK_OBJ(cont);
    CHECK_OBJ(arg);
    if (!continuation3_ops.mo_super)
	mem_mixvec_create_ops(&continuation3_ops, L"contination3", 1, 2);
    return make_mixvec_1_2(&continuation3_ops, (word_t)proc, cont, arg);
}

obj_t make_continuation4(cont_proc_t proc, obj_t cont, obj_t arg1, obj_t arg2)
{
    CHECK_OBJ(cont);
    CHECK_OBJ(arg1);
    CHECK_OBJ(arg2);
    if (!continuation4_ops.mo_super)
	mem_mixvec_create_ops(&continuation4_ops, L"contination4", 1, 3);
    return make_mixvec_1_3(&continuation4_ops, (word_t)proc, cont, arg1, arg2);
}
