#include "env.h"
#include "prim.h"
#include "list.h"
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

static cv_t c_continue_if(obj_t cont, obj_t values)
{
    EVAL_LOG("values=%O", values);
    //oprintf("c_continue_if: values = %O\n", values);
    bool success = CAR(values) != FALSE_OBJ;
    obj_t form   = cont4_arg(cont);
    obj_t env    = cont_env(cont);
    if (success) {
	obj_t consequent = CADDR(form);
	return cv(make_cont4(c_eval, cont_cont(cont), env, consequent),
		  CDR(values));
    } else if (is_null(CDDDR(form)))
	return cv(cont_cont(cont), CONS(UNDEFINED_OBJ, CDR(values)));
    else {
	obj_t alternate = CADDDR(form);
	return cv(make_cont4(c_eval, cont_cont(cont), env, alternate),
		  CDR(values));
    }
}

DEFINE_SPECIAL_FORM(L"if")(obj_t cont, obj_t values)
{
    obj_t form = cont4_arg(cont);
    EVAL_LOG("form=%O", form);
    size_t len = list_length(form);
    CHECK(len >= 3 && len <= 4, "if takes 2 or 3 arguments");
    obj_t test = CADR(form);
    obj_t env = cont_env(cont);
    obj_t second = make_cont4(c_continue_if, cont_cont(cont), env, form);
    obj_t first = make_cont4(c_eval, second, env, test);
    return cv(first, CDR(values));
}

static cv_t c_continue_set(obj_t cont, obj_t values)
{
    obj_t form  = cont4_arg(cont);
    obj_t env   = cont_env(cont);
    obj_t var   = CADR(form);
    obj_t value = CAR(values);
    obj_t bdg   = env_lookup(env, var);
    EVAL_LOG("var=%O value=%O", var, value);
    /* N.B., allocate values list before mutating environment. */
    obj_t new_values = CONS(UNDEFINED_OBJ, CDR(values));
    binding_set_value(bdg, value);
    return cv(cont_cont(cont), new_values);
}

DEFINE_SPECIAL_FORM(L"set!")(obj_t cont, obj_t values)
{
    obj_t form = cont4_arg(cont);
    obj_t env = cont_env(cont);
    EVAL_LOG("form=%O", form);
    CHECK(list_length(form) == 3, "set! takes 2 arguments");
    obj_t expr = CADDR(form);
    obj_t second = make_cont4(c_continue_set, cont_cont(cont), env, form);
    obj_t first = make_cont4(c_eval, second, env, expr);
    return cv(first, EMPTY_LIST);
}
