#include "obj_pair.h"

#include "except.h"
#include "mem_fixvec.h"

mem_ops_t pair_ops;

extern obj_t make_pair(obj_t car, obj_t cdr)
{
    CHECK_OBJ(car);
    CHECK_OBJ(cdr);
    if (!pair_ops.mo_super)
	mem_fixvec_create_ops(&pair_ops, L"pair", 2);
    return make_fixvec2(&pair_ops, car, cdr);
}

obj_t pair_car(obj_t pair)
{
    CHECK_OBJ(pair);
    CHECK(is_pair(pair), NULL, "must be pair", pair);
    return fixvec2_get_ptr(pair, 0);
}

obj_t pair_cdr(obj_t pair)
{
    CHECK_OBJ(pair);
    CHECK(is_pair(pair), NULL, "must be pair", pair);
    return fixvec2_get_ptr(pair, 1);
}

void pair_set_car(obj_t pair, obj_t car)
{
    CHECK_OBJ(pair);
    CHECK_OBJ(car);
    //XXX CHECK(is_mutable(pair), NULL, "must be mutable", pair);
    CHECK(is_pair(pair), NULL, "must be pair", pair);
    fixvec2_set_ptr(pair, 0, car);
}

void pair_set_cdr(obj_t pair, obj_t cdr)
{
    CHECK_OBJ(pair);
    CHECK_OBJ(cdr);
    //XXX CHECK(is_mutable(pair), NULL, "must be mutable", pair);
    CHECK(is_pair(pair), NULL, "must be pair", pair);
    fixvec2_set_ptr(pair, 1, cdr);
}
