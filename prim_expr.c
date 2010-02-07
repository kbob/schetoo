#include "prim.h"
#include "types.h"

DEFINE_COOKED_SPECIAL_FORM(L"quote", 1)(obj_t datum)
{
    return datum;
}

DEFINE_SPECIAL_FORM(L"lambda")(obj_t cont, obj_t values)
{
    obj_t expr    = cont4_arg(cont);
    obj_t formals = CADR(expr);
    obj_t body    = CDDR(expr);
    return cv(cont_cont(cont),
	      CONS(make_procedure(body, formals, cont_env(cont)), CDR(values)));
}
