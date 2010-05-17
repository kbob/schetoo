#ifndef OBJ_UNSPEC_INCLUDED
#define OBJ_UNSPEC_INCLUDED

#include "obj.h"

#define UNSPECIFIED_REPR L"#<unspecified>"

static inline obj_t make_unspecified(void)
{
    return UNSPECIFIED_OBJ;
}

static inline bool is_unspecified(obj_t obj)
{
    return obj == UNSPECIFIED_OBJ;
}

#endif /* !OBJ_UNSPEC_INCLUDED */
