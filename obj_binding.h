#ifndef OBJ_BINDING_INCLUDED
#define OBJ_BINDING_INCLUDED

#include "except.h"
#include "mem_mixvec.h"
#include "obj.h"

typedef enum binding_type {
    BT_CORE,
    BT_LEXICAL,
    BT_MACRO,
    BT_PATTERN
} binding_type_t;

typedef enum mutability {
    M_IMMUTABLE,
    M_MUTABLE,
} mutability_t;

OBJ_TYPE_PREDICATE(binding);		// bool is_binding(obj_t);

extern obj_t	      make_binding         (obj_t          name,
					    binding_type_t type,
					    mutability_t   is_mutable,
					    obj_t          value);
static inline obj_t binding_name(obj_t bdg)
{
    CHECK(is_binding(bdg), NULL, "must be binding", bdg);
    return mixvec_1_2_get_ptr(bdg, 0);
}

static inline binding_type_t binding_type(obj_t bdg)
{
    CHECK(is_binding(bdg), NULL, "must be binding", bdg);
    return mixvec_1_2_get_int(bdg, 0) >> 1;
}

static inline bool binding_is_mutable(obj_t bdg)
{
    CHECK(is_binding(bdg), NULL, "must be binding", bdg);
    return mixvec_1_2_get_int(bdg, 0) & 1;
}

static inline obj_t binding_value(obj_t bdg)
{
    CHECK(is_binding(bdg), NULL, "must be binding", bdg);
    return mixvec_1_2_get_ptr(bdg, 1);
}

extern void binding_set_mutability(obj_t, mutability_t);
extern void binding_set_value     (obj_t, obj_t value);

#endif /* !OBJ_BINDING_INCLUDED */
