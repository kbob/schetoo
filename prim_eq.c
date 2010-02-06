#include "prim.h"
#include "types.h"

DEFINE_PROC(L"eq?", 2)(obj_t obj1, obj_t obj2)
{
    return make_boolean(obj1 == obj2);
}
