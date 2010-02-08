#include "prim.h"
#include "types.h"

DEFINE_PROC(L"procedure?", 1)(obj_t obj)
{
    return make_boolean(is_procedure(obj));
}
