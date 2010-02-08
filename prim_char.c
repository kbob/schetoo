#include "prim.h"
#include "types.h"

static inline bool is_Unicode_scalar_value(int n)
{
    return (0 <= n && n <= 0xd7ff) || (0xe000 <= n && n <= 0x10ffff);
}

DEFINE_PROC(L"char?", 1)(obj_t obj)
{
    return make_boolean(is_character(obj));
}

DEFINE_PROC(L"char->integer", 1)(obj_t chr)
{
    return make_fixnum(character_value(chr));
}

DEFINE_PROC(L"integer->char", 1)(obj_t sv)
{
    word_t i = fixnum_value(sv);
    CHECK(is_Unicode_scalar_value(i), NULL, "must be Unicode scalar value", sv);
    return make_character(i);
}
