#include "obj_cont.h"

#include "mem_mixvec.h"

mem_ops_t cont3_ops;

obj_t make_continuation3(cont_proc_t proc, obj_t cont, obj_t arg)
{
    if (!cont3_ops.mo_super)
	mem_mixvec_create_ops(&cont3_ops, L"contination3", 1, 2);
    obj_t r = alloc_mixvec_1_2(&cont3_ops);
    mixvec_1_2_set_int(r, 0, (word_t)proc);
    mixvec_1_2_set_ptr(r, 0, cont);
    mixvec_1_2_set_ptr(r, 1, arg);
    return r;
}
