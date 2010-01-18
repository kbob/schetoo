#ifndef OBJ_UNDEF_INCLUDED
#define OBJ_UNDEF_INCLUDED

#include "obj.h"

static inline obj_t make_undefined(void)
{
    return UNDEFINED;
}

static inline bool is_undefined(obj_t obj)
{
    return obj == UNDEFINED;
}

#endif /* !OBJ_UNDEF_INCLUDED */
