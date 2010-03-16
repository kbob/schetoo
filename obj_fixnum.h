#ifndef OBJ_FIXNUM_INCLUDED
#define OBJ_FIXNUM_INCLUDED

#include "except.h"
#include "obj.h"

static inline obj_t make_fixnum(word_t value)
{
    return (obj_t)(value << FIXNUM_SHIFT | FIXNUM_TAG);
}

static inline bool is_fixnum(obj_t obj)
{
    return (obj_bits(obj) & FIXNUM_TAG_MASK) == FIXNUM_TAG;
}

static inline word_t fixnum_value(obj_t n)
{
    CHECK(is_fixnum(n), "must be fixnum", n);
    return obj_bits(n) >> FIXNUM_SHIFT;
}

#endif /* !OBJ_FIXNUM_INCLUDED */
