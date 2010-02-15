#include "env.h"
#include "prim.h"
#include "types.h"

DEFINE_RAW_PROC(L"interaction-environment")(obj_t cont, obj_t values)
{
    obj_t arg = cont4_arg(cont);
    return cv(cont_cont(cont), CONS(cont_env(cont), CDR(arg)));
}

DEFINE_PROC(L"root-environment", 0)(void)
{
    return root_environment();
}