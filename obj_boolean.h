#ifndef OBJ_BOOL_INCLUDED
#define OBJ_BOOL_INCLUDED

#include "except.h"
#include "obj.h"

#define FALSE_OBJ (BOOLEAN_OBJ(0))
#define TRUE_OBJ  (BOOLEAN_OBJ(1))

static inline obj_t make_boolean(bool value)
{
    return value ? TRUE_OBJ : FALSE_OBJ;
}

static inline bool is_boolean(const obj_t o)
{
    return (obj_bits(o) & LONG_TAG_MASK) == BOOLEAN_TAG;
}

static inline bool boolean_value(obj_t boolean)
{
    CHECK(is_boolean(boolean), "must be Boolean", boolean);
    return boolean == TRUE_OBJ;
}

#endif /* !OBJ_BOOL_INCLUDED */
