#include "env.h"
#include "list.h"
#include "prim.h"
#include "types.h"

static cv_t c_continue_define(obj_t cont, obj_t values)
{
    EVAL_LOG("var=%O values=%O", cont4_arg(cont), values);
    /* N.B., allocate new values before mutating environment. */
    obj_t new_values = CONS(UNDEFINED_OBJ, CDR(cont4_arg(cont)));
    env_bind(cont_env(cont),
	     CAR(cont4_arg(cont)),
	     BT_LEXICAL,
	     M_MUTABLE,
	     CAR(values));
    return cv(cont_cont(cont), new_values);
}

DEFINE_SPECIAL_FORM(L"define")(obj_t cont, obj_t values)
{
    obj_t form = cont4_arg(cont);
    obj_t env  = cont_env(cont);
    EVAL_LOG("form=%O", form);
    CHECK(list_length(form) == 3, "define takes 2 arguments");
    obj_t var  = CADR(form);
    obj_t expr = CADDR(form);
    obj_t second = make_cont4(c_continue_define,
			      cont_cont(cont),
			      env,
			      CONS(var, CDR(values)));
    obj_t first = make_cont4(c_eval, second, env, expr);
    return cv(first, EMPTY_LIST);
}
