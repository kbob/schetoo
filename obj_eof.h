#ifndef OBJ_EOF_INCLUDED
#define OBJ_EOF_INCLUDED

#include "mem.h"

static inline obj_t make_eof(void)
{
    return END_OF_FILE;
}

static inline bool is_eof(obj_t *obj)
{
    return obj == END_OF_FILE;
}

#endif /* !OBJ_EOF_INCLUDED */
