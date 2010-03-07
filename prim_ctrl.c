#include "list.h"
#include "prim.h"
#include "types.h"

DEFINE_RAW_PROC(L"apply")(obj_t cont, obj_t values)
{
    obj_t expr = cont4_arg(cont);
    EVAL_LOG("expr=%O values=%O", expr, values);
    obj_t arg_list = CAR(values);
    obj_t p = CDR(values);
    while (!is_null(CDR(p))) {
	arg_list = CONS(CAR(p), arg_list);
	p = CDR(p);
    }
    obj_t operator = CAR(p);
    EVAL_LOG("operator=%O arg_list=%O", operator, arg_list);
    
    return cv(make_cont4(c_apply_proc,
			 cont_cont(cont),
			 cont_env(cont),
			 CONS(operator, CDR(expr))),
	      reverse_list(arg_list));
}

static cv_t escape_callcc(obj_t cont, obj_t values)
{
    obj_t self = CAR(cont4_arg(cont));
    obj_t escape_cont = CAR(procedure_env(self));
    obj_t escape_values = CDR(procedure_env(self));
    return cv(escape_cont, CONS(CAR(values), escape_values));
}

DEFINE_RAW_PROC(L"call-with-current-continuation")(obj_t cont, obj_t values)
{
    obj_t saved_values = CDR(cont4_arg(cont));
    obj_t closure = CONS((obj_t)cont_cont(cont),
			 saved_values);
    obj_t escape = make_raw_procedure(escape_callcc, UNDEFINED_OBJ, closure);
    return cv(make_cont4(c_apply_proc,
			 cont_cont(cont),
			 cont_env(cont),
			 CONS(CAR(values), CDR(cont4_arg(cont)))),
	      CONS(escape, EMPTY_LIST));
}

ALIAS_NAME(L"call-with-current-continuation", L"call/cc");
