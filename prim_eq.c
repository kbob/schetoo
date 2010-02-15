#include "prim.h"
#include "types.h"

/* Because of the way fixnums, Booleans, and characters are encoded in
 * pointers, eqv? and eq? share the same implementation.
 *
 * This leads to the amusing result that (eq? eq? eqv?) returns #t.
 */

DEFINE_PROC(L"eqv?", 2)(obj_t obj1, obj_t obj2)
{
    return make_boolean(obj1 == obj2);
}

ALIAS_NAME(L"eqv?", L"eq?");
