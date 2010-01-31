#include "obj_binding.h"

mem_ops_t binding_ops;

static inline int combine(binding_type_t type, mutability_t mutability)
{
    return type << 1 | mutability;
}

static inline binding_type_t extract_type(int combined)
{
    return combined >> 1;
}

static inline mutability_t extract_mutability(int combined)
{
    return combined & 1;
}

obj_t make_binding(obj_t          name,
		   binding_type_t type,
		   mutability_t   mutability,
		   obj_t          value)
{
    CHECK_OBJ(name);
    CHECK_OBJ(value);
    ASSERT(type == BT_CORE  || type == BT_LEXICAL ||
	   type == BT_MACRO || type == BT_PATTERN);
    ASSERT(mutability == M_MUTABLE || mutability == M_IMMUTABLE);

    if (!binding_ops.mo_super)
	mem_mixvec_create_ops(&binding_ops, L"binding", 1, 2);
    return make_mixvec_1_2(&binding_ops,
			   combine(type, mutability),
			   name,
			   value);
}

void binding_set_mutability(obj_t bdg, mutability_t mutability)
{
    CHECK_OBJ(bdg);
    CHECK(is_binding(bdg), bdg, "binding-set-mutability: must be binding");
    mixvec_1_2_set_int(bdg, 0, combine(binding_type(bdg), mutability));
}

void binding_set_value(obj_t bdg, obj_t value)
{
    CHECK_OBJ(bdg);
    CHECK_OBJ(value);
    CHECK(is_binding(bdg), bdg, "binding-set-value: must be binding");
    CHECK(binding_is_mutable(bdg), bdg, "binding-set-value: must be mutable");
    mixvec_1_2_set_ptr(bdg, 1, value);
}
