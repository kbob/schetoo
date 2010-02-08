#include "env.h"
#include "list.h"
#include "oprintf.h"
#include "prim.h"
#include "types.h"

static cv_t c_continue_define(obj_t cont, obj_t values)
{
    /* N.B., allocate new values before mutating environment. */
    obj_t new_values = CONS(UNDEFINED_OBJ, CDR(values));
    env_bind(cont_env(cont),
	     cont4_arg(cont),
	     BT_LEXICAL,
	     M_MUTABLE,
	     CAR(values));
    return cv(cont_cont(cont), new_values);
}

DEFINE_SPECIAL_FORM(L"define")(obj_t cont, obj_t values)
{
    obj_t form = cont4_arg(cont);
    obj_t env  = cont_env(cont);
    //oprintf("define  \tform=%O\n", form);
    CHECK(list_length(form) == 3,
	  make_symbol_from_C_str(L"define"),
	  "define must have exactly two args", form);
    obj_t var  = CADR(form);
    obj_t expr = CADDR(form);
    obj_t second = make_cont4(c_continue_define, cont_cont(cont), env, var);
    obj_t first = make_cont4(c_eval, second, env, expr);
    return cv(first, EMPTY_LIST);
}
