#include "obj_pair.h"

#include "except.h"
#include "mem_fixvec.h"

static mem_ops_t pair_ops;

extern obj_t make_pair(obj_t car, obj_t cdr)
{
    check_obj(car);
    check_obj(cdr);
    if (!pair_ops.mo_super)
	mem_fixvec_create_ops(&pair_ops, L"pair", 2);
    return alloc_fixvec2(&pair_ops, car, cdr);
}

bool is_pair(obj_t obj)
{
    check_obj(obj);
    return is_normal(obj) && obj_mem_ops(obj) == &pair_ops;
}

obj_t pair_car(obj_t pair)
{
    check_obj(pair);
    CHECK(is_pair(pair), pair, "car: pair required");
    return fixvec2_get_ptr(pair, 0);
}

obj_t pair_cdr(obj_t pair)
{
    check_obj(pair);
    CHECK(is_pair(pair), pair, "cdr: pair required");
    return fixvec2_get_ptr(pair, 1);
}

void pair_set_car(obj_t pair, obj_t car)
{
    check_obj(pair);
    check_obj(car);
    CHECK(is_pair(pair), pair, "set-car!: pair required");
    fixvec2_set_ptr(pair, 0, car);
}

void pair_set_cdr(obj_t pair, obj_t cdr)
{
    check_obj(pair);
    check_obj(cdr);
    CHECK(is_pair(pair), pair, "set-cdr!: pair required");
    fixvec2_set_ptr(pair, 1, cdr);
}
