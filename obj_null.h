#ifndef OBJ_NULL_INCLUDED
#define OBJ_NULL_INCLUDED

#include "obj.h"

static inline obj_t make_null(void)
{
    return EMPTY_LIST;
}

static inline bool is_null(obj_t obj)
{
    return obj == EMPTY_LIST;
}

#endif /* !OBJ_NULL_INCLUDED */
