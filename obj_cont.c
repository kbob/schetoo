#include "obj_cont.h"

mem_ops_t cont3_ops;
mem_ops_t cont4_ops;			/* XXX not used yet */

obj_t make_cont3(cont_proc_t proc, obj_t cont, obj_t arg)
{
    CHECK_OBJ(cont);
    CHECK_OBJ(arg);
    if (!cont3_ops.mo_super)
	mem_mixvec_create_ops(&cont3_ops, L"cont3", 1, 2);
    return make_mixvec_1_2(&cont3_ops, (word_t)proc, cont, arg);
}

obj_t make_cont4(cont_proc_t proc, obj_t cont, obj_t arg1, obj_t arg2)
{
    CHECK_OBJ(cont);
    CHECK_OBJ(arg1);
    CHECK_OBJ(arg2);
    if (!cont4_ops.mo_super)
	mem_mixvec_create_ops(&cont4_ops, L"cont4", 1, 3);
    return make_mixvec_1_3(&cont4_ops, (word_t)proc, cont, arg1, arg2);
}
