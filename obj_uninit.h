#ifndef OBJ_UNINIT_INCLUDED
#define OBJ_UNINIT_INCLUDED

#include "obj.h"

#define UNINITIALIZED_REPR L"#<uninitialized>"

static inline obj_t make_uninitialized(void)
{
    return UNINITIALIZED_OBJ;
}

static inline bool is_uninitialized(obj_t obj)
{
    return obj == UNINITIALIZED_OBJ;
}

#endif /* !OBJ_UNINIT_INCLUDED */
