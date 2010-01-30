#include "obj_cont.h"

#include "mem_mixvec.h"

mem_ops_t cont3_ops;

obj_t make_continuation3(cont_proc_t proc, obj_t cont, obj_t arg)
{
    CHECK_OBJ(cont);
    CHECK_OBJ(arg);
    if (!cont3_ops.mo_super)
	mem_mixvec_create_ops(&cont3_ops, L"contination3", 1, 2);
    return make_mixvec_1_2(&cont3_ops, (word_t)proc, cont, arg);
}
