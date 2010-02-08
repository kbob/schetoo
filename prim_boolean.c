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

DEFINE_PROC(L"boolean=?", 2-)(obj_t bool1, obj_t bool2, obj_t rest)
{
    CHECK(is_boolean(bool1), make_symbol_from_C_str(L"boolean=?"),
	  "must be Boolean", bool1);
    CHECK(is_boolean(bool2), make_symbol_from_C_str(L"boolean=?"),
	  "must be Boolean", bool2);
    if (bool2 != bool1)
	return FALSE_OBJ;
    while (!is_null(rest)) {
	obj_t boo = CAR(rest);
	CHECK(is_boolean(boo), make_symbol_from_C_str(L"boolean=?"),
	      "must be Boolean", boo);
	if (boo != bool1)
	    return FALSE_OBJ;
	rest = CDR(rest);
    }
    return TRUE_OBJ;
}
