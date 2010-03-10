#include "env.h"
#include "prim.h"
#include "test.h"
#include "types.h"

DEFINE_RAW_PROC(L"interaction-environment")(obj_t cont, obj_t values)
{
    assert(is_cont5(cont));
    obj_t saved_values = cont5_arg2(cont);
    return cv(cont_cont(cont), CONS(cont_env(cont), saved_values));
}

DEFINE_PROC(L"root-environment", 0)(void)
{
    return root_environment();
}

TEST_EVAL(L"(pair? (interaction-environment))",	L"#t");
