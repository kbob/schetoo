#include "obj_pair.h"

#include "except.h"

mem_ops_t pair_ops;

extern obj_t make_pair(obj_t car, obj_t cdr)
{
    CHECK_OBJ(car);
    CHECK_OBJ(cdr);
    if (!pair_ops.mo_super)
	mem_fixvec_create_ops(&pair_ops, L"pair", 2);
    return make_fixvec2(&pair_ops, car, cdr);
}

void pair_set_car_nc(obj_t pair, obj_t car)
{
    CHECK_OBJ(pair);
    CHECK_OBJ(car);
    fixvec2_set_ptr(pair, 0, car);
}

void pair_set_car(obj_t pair, obj_t car)
{
    CHECK_OBJ(pair);
    CHECK_OBJ(car);
    //XXX CHECK(is_mutable(pair), "must be mutable", pair);
    CHECK(is_pair(pair), "must be pair", pair);
    fixvec2_set_ptr(pair, 0, car);
}

void pair_set_cdr_nc(obj_t pair, obj_t cdr)
{
    CHECK_OBJ(pair);
    CHECK_OBJ(cdr);
    fixvec2_set_ptr(pair, 1, cdr);
}

void pair_set_cdr(obj_t pair, obj_t cdr)
{
    CHECK_OBJ(pair);
    CHECK_OBJ(cdr);
    //XXX CHECK(is_mutable(pair), "must be mutable", pair);
    CHECK(is_pair(pair), "must be pair", pair);
    fixvec2_set_ptr(pair, 1, cdr);
}
