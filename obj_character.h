#ifndef OBJ_CHARACTER_INCLUDED
#define OBJ_CHARACTER_INCLUDED

#include "char.h"
#include "obj.h"
#include "except.h"

static inline obj_t make_character(char_t value)
{
    return (obj_t)(word_t)(value << CHARACTER_SHIFT | CHARACTER_TAG);
}

static inline bool is_character(const obj_t o)
{
    return (obj_bits(o) & LONG_TAG_MASK) == CHARACTER_TAG;
}

static inline char_t character_value(obj_t ch)
{
    CHECK(is_character(ch), "must be character", ch);
    return obj_bits(ch) >> CHARACTER_SHIFT;
}

#endif /* !OBJ_CHARACTER_INCLUDED */
