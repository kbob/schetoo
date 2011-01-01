#include "prim.h"
#include "test.h"
#include "types.h"

DEFINE_PROC(L"string?", 1)(obj_t obj)
{
    return make_boolean(is_string(obj));
}

DEFINE_PROC(L"string->list", 1)(obj_t string)
{
    const char_t *chars = string_value(string);
    obj_t list = EMPTY_LIST;
    for (size_t pos = string_len(string); pos; --pos)
	list = make_pair(make_character(chars[pos - 1]), list);
    return list;
}

TEST_EVAL(L"(string->list \"abc\")",    L"(#\\a #\\b #\\c)");
