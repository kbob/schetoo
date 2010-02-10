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
