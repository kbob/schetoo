#ifndef OBJ_BOOL_INCLUDED
#define OBJ_BOOL_INCLUDED

#include "except.h"
#include "obj.h"

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
    CHECK(is_boolean(boolean), boolean, "boolean_value: must be Boolean");
    return boolean == TRUE_OBJ;
}

#endif /* !OBJ_BOOL_INCLUDED */
