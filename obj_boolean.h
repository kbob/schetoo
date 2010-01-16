#ifndef OBJ_BOOL_INCLUDED
#define OBJ_BOOL_INCLUDED

#include "mem.h"

static inline obj_t make_boolean(bool value)
{
    return value ? TRUE_OBJ : FALSE_OBJ;
}

static inline bool boolean_value(obj_t boolean)
{
    CHECK(is_boolean(boolean), boolean, "boolean_value: must be Boolean");
    return boolean == TRUE_OBJ;
}

#endif /* !OBJ_BOOL_INCLUDED */
