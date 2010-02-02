#include "proc.h"
#include "types.h"

DEFINE_PROC(L"pair?", 1)(obj_t obj)
{
    return make_boolean(is_pair(obj));
}

DEFINE_PROC(L"cons", 2)(obj_t obj1, obj_t obj2)
{
    return CONS(obj1, obj2);
}

DEFINE_PROC(L"car", 1)(obj_t pair)
{
    return CAR(pair);
}

DEFINE_PROC(L"cdr", 1)(obj_t pair)
{
    return CDR(pair);
}

DEFINE_PROC(L"null?", 1)(obj_t obj)
{
    return make_boolean(is_null(obj));
}

DEFINE_PROC(L"list", 0-)(obj_t arg_list)
{
    return arg_list;
}
