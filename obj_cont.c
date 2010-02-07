#include "obj_cont.h"

mem_ops_t cont4_ops;

obj_t make_cont4(cont_proc_t proc, obj_t cont, obj_t env, obj_t arg)
{
    CHECK_OBJ(cont);
    CHECK_OBJ(env);
    CHECK_OBJ(arg);
    if (!cont4_ops.mo_super)
	mem_mixvec_create_ops(&cont4_ops, L"cont4", 1, 3);
    return make_mixvec_1_3(&cont4_ops, (word_t)proc, cont, env, arg);
}
