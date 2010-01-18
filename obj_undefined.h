#ifndef OBJ_UNDEF_INCLUDED
#define OBJ_UNDEF_INCLUDED

#include "obj.h"

static inline obj_t make_undefined(void)
{
    return UNDEFINED_OBJ;
}

static inline bool is_undefined(obj_t obj)
{
    return obj == UNDEFINED_OBJ;
}

#endif /* !OBJ_UNDEF_INCLUDED */
