#include "list.h"
#include "prim.h"
#include "types.h"

DEFINE_RAW_PROC(L"eval")(obj_t cont, obj_t values)
{
    obj_t arg = cont4_arg(cont);
    size_t len = list_length(values);
    CHECK(len >= 2, make_symbol_from_C_str(L"eval"), "too few args");
    CHECK(len <= 2, make_symbol_from_C_str(L"eval"), "too many args");
    obj_t env = CAR(values);
    obj_t expr = CADR(values);
    return cv(make_cont4(c_eval, cont_cont(cont), env, expr), CDR(arg));
}
