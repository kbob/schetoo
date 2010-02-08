#include "prim.h"
#include "types.h"

DEFINE_PROC(L"symbol?", 1)(obj_t obj)
{
    return make_boolean(is_symbol(obj));
}

DEFINE_PROC(L"symbol->string", 1)(obj_t symbol)
{
    return symbol_name(symbol);
}

DEFINE_PROC(L"string->symbol", 1)(obj_t string)
{
    return make_symbol(string);
}
