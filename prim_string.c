#include "prim.h"
#include "types.h"

DEFINE_PROC(L"string?", 1)(obj_t obj)
{
    return make_boolean(is_string(obj));
}
