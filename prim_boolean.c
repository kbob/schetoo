#include "prim.h"
#include "types.h"

DEFINE_PROC(L"not", 1)(obj_t obj)
{
    return make_boolean(obj == FALSE_OBJ);
}

DEFINE_PROC(L"boolean?", 1)(obj_t obj)
{
    return make_boolean(is_boolean(obj));
}

DEFINE_PROC(L"boolean=?", 2-)(obj_t bool1, obj_t bool2, obj_t bools)
{
    CHECK(is_boolean(bool1), NULL, "must be Boolean");
    while (true) {
	CHECK(is_boolean(bool2), NULL, "must be Boolean");
	if (!(bool2 == bool1))
	    return FALSE_OBJ;
	if (is_null(bools))
	    return TRUE_OBJ;
	bool1 = bool2;
	bool2 = CAR(bools);
	bools = CDR(bools);
    }
}
