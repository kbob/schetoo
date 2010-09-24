#include "obj_env_rec.h"

mem_ops_t env_rec_ops;

obj_t make_env_rec(obj_t parent, int nvar, obj_t formals, obj_t actuals)
{
    CHECK_OBJ(parent);
    CHECK_OBJ(formals);
    CHECK_OBJ(actuals);
    if (!env_rec_ops.mo_super)
	mem_mixvec_create_ops(&env_rec_ops, L"environment", 1, 3);
    return make_mixvec_1_3(&env_rec_ops, nvar, parent, formals, actuals);
}

