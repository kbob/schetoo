#include "obj_cont.h"

mem_ops_t cont3_ops;
mem_ops_t cont4_ops;
mem_ops_t cont5_ops;
mem_ops_t cont6_ops;

obj_t make_cont3(cont_proc_t proc, obj_t cont, obj_t env)
{
    CHECK_OBJ(cont);
    CHECK_OBJ(env);
    if (!cont3_ops.mo_super)
	mem_mixvec_create_ops(&cont3_ops, L"continuation", 1, 2);
    return make_mixvec_1_2(&cont3_ops, (word_t)proc, cont, env);
}

obj_t make_cont4(cont_proc_t proc, obj_t cont, obj_t env, obj_t arg)
{
    CHECK_OBJ(cont);
    CHECK_OBJ(env);
    CHECK_OBJ(arg);
    if (!cont4_ops.mo_super)
	mem_mixvec_create_ops(&cont4_ops, L"continuation", 1, 3);
    return make_mixvec_1_3(&cont4_ops, (word_t)proc, cont, env, arg);
}

obj_t make_cont5(cont_proc_t proc,
		 obj_t cont, obj_t env,
		 obj_t arg1, obj_t arg2)
{
    CHECK_OBJ(cont);
    CHECK_OBJ(env);
    CHECK_OBJ(arg1);
    CHECK_OBJ(arg2);
    if (!cont5_ops.mo_super)
	mem_mixvec_create_ops(&cont5_ops, L"continuation", 1, 4);
    return make_mixvec_1_4(&cont5_ops, (word_t)proc, cont, env, arg1, arg2);
}

obj_t make_cont6(cont_proc_t proc,
		 obj_t cont, obj_t env,
		 obj_t arg1, obj_t arg2, obj_t arg3)
{
    CHECK_OBJ(cont);
    CHECK_OBJ(env);
    CHECK_OBJ(arg1);
    CHECK_OBJ(arg2);
    CHECK_OBJ(arg3);
    if (!cont6_ops.mo_super)
	mem_mixvec_create_ops(&cont6_ops, L"continuation", 1, 5);
    return make_mixvec_1_5(&cont6_ops, (word_t)proc, cont, env,
			   arg1, arg2, arg3);
}
