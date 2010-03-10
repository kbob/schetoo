#include "list.h"
#include "prim.h"
#include "test.h"
#include "types.h"

DEFINE_RAW_PROC(L"eval")(obj_t cont, obj_t values)
{
    assert(is_cont5(cont));
    obj_t saved_values = cont5_arg2(cont);
    size_t len = list_length(values);
    CHECK(len == 2, "eval takes 2 arguments");
    obj_t env = CAR(values);
    obj_t expr = CADR(values);
    return cv(make_cont4(c_eval, cont_cont(cont), env, expr), saved_values);
}

TEST_EVAL(L"(eval '(+ 3 4) (root-environment))", L"7");
