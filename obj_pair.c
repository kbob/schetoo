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
    return alloc_fixvec2(&pair_ops, car, cdr);
}

obj_t pair_car(obj_t pair)
{
    CHECK_OBJ(pair);
    CHECK(is_pair(pair), pair, "car: pair required");
    return fixvec2_get_ptr(pair, 0);
}

obj_t pair_cdr(obj_t pair)
{
    CHECK_OBJ(pair);
    CHECK(is_pair(pair), pair, "cdr: pair required");
    return fixvec2_get_ptr(pair, 1);
}

void pair_set_car(obj_t pair, obj_t car)
{
    CHECK_OBJ(pair);
    CHECK_OBJ(car);
    CHECK(is_pair(pair), pair, "set-car!: pair required");
    fixvec2_set_ptr(pair, 0, car);
}

void pair_set_cdr(obj_t pair, obj_t cdr)
{
    CHECK_OBJ(pair);
    CHECK_OBJ(cdr);
    CHECK(is_pair(pair), pair, "set-cdr!: pair required");
    fixvec2_set_ptr(pair, 1, cdr);
}
