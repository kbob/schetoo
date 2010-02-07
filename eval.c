#include "eval.h"

#include <setjmp.h>
#include <stdio.h>

#include "env.h"
#include "except.h"
#include "heap.h"
#include "low_ex.h"
#include "obj_cont.h"
#include "oprintf.h"
#include "prim.h"
#include "roots.h"
#include "types.h"

/*
 * I'm having trouble figuring out exactly what values is.
 *
 * I think it's a list of computed values in the current argument
 * list.  Except sometimes (c_eval_operator) it's just the list of the
 * operator.
 */

static __thread sigjmp_buf eval_sigrestart;
static __thread jmp_buf    eval_restart;

THREAD_ROOT(cont_root);
THREAD_ROOT(values_root);

static inline bool is_self_evaluating(obj_t expr)
{
    return is_boolean(expr)   ||
	   is_fixnum(expr)    ||
	   is_character(expr) ||
	   is_string(expr)    ;
}

static inline bool is_application(obj_t expr)
{
    return is_pair(expr);
}

static inline obj_t application_operator(obj_t expr)
{
    return CAR(expr);
}

static inline obj_t application_operands(obj_t expr)
{
    return CDR(expr);
}

// XXX put this somewhere public.
static obj_t reverse_list(obj_t list)
{
    obj_t rev = EMPTY_LIST;
    while (!is_null(list)) {
	rev = CONS(CAR(list), rev);
	list = CDR(list);
    }
    return rev;
}

static cv_t c_eval(obj_t cont, obj_t values);

static cv_t c_eval_seq(obj_t cont, obj_t values)
{
    obj_t env = cont_env(cont);
    obj_t exprs = cont4_arg(cont);
    //oprintf("c_eval_seq\texprs=%O\n", exprs);
    obj_t second;
    if (is_null(CDR(exprs)))
	second = cont_cont(cont);
    else
	second = make_cont4(c_eval_seq, cont_cont(cont), env, CDR(exprs));
    obj_t first = make_cont4(c_eval, second, env, CAR(exprs));
    return cv(first, EMPTY_LIST);
}

static cv_t c_apply_proc(obj_t cont, obj_t values)
{
    obj_t p = cont4_arg(cont);
    obj_t next = cont_cont(cont);
    obj_t operator = CAR(p);
    obj_t saved_values = CDR(p);
    //oprintf("c_apply_proc\top=%O values=%O\n", operator, values);
    obj_t arg_list = reverse_list(values);
    if (procedure_is_C(operator)) {
	if (procedure_is_raw(operator))
	    ASSERT(false && "implement raw procedure application");
	else
	    return cv(next, CONS(apply_proc(operator, arg_list), saved_values));
    } else {
	obj_t new_env = make_env(procedure_env(operator));
	obj_t body    = procedure_body(operator);
	obj_t formals = procedure_args(operator);
	obj_t actuals = arg_list;
	obj_t formal;
	obj_t actual;
	while (!is_null(formals) || !is_null(actuals)) {
	    CHECK(!is_null(formals), operator, "too many arguments", arg_list);
	    if (is_pair(formals)) {
		CHECK(!is_null(actuals),
		      operator, "not enough arguments", arg_list);
		formal  = CAR(formals);
		formals = CDR(formals);
		actual  = CAR(actuals);
		actuals = CDR(actuals);
	    } else {
		formal  = formals;
		actual  = actuals;
		formals = actuals = EMPTY_LIST;
	    }
	    env_bind(new_env, formal, BT_LEXICAL, M_MUTABLE, actual);
	}
	return cv(make_cont4(c_eval_seq, next, new_env, body), EMPTY_LIST);
    }
}

static cv_t c_eval_operator(obj_t cont, obj_t values)
{
    obj_t appl = cont4_arg(cont);
    obj_t operator = CAR(values);
    //oprintf("c_eval_operator\tappl = %O; operator = %O\n", appl, operator);
    if (!is_procedure(operator)) {
	raise(&syntax, NULL, "must be procedure", operator);
    }
    if (!procedure_args_evaluated(operator)) {
	ASSERT(procedure_is_C(operator) && "implement Scheme special forms");
	if (procedure_is_raw(operator)) {
	    return ((cont_proc_t)procedure_code(operator))(cont, values);
	} else {
	    obj_t arg_list = application_operands(appl);
	    return cv(cont_cont(cont),
		      CONS(apply_proc(operator, arg_list), CDR(values)));
	}
    }
    obj_t arg_list = reverse_list(application_operands(appl));
    cont = make_cont4(c_apply_proc,
		      cont_cont(cont),
		      cont_env(cont),
		      CONS(operator, CDR(values)));
    while (!is_null(arg_list)) {
	cont = make_cont4(c_eval, cont, cont_env(cont), CAR(arg_list));
	arg_list = CDR(arg_list);
    }
    return cv(cont, EMPTY_LIST);
}

static cv_t c_eval(obj_t cont, obj_t values)
{
    obj_t expr = cont4_arg(cont);
    //oprintf("c_eval  \texpr=%O\n", expr);
    if (is_self_evaluating(expr))
	return cv(cont_cont(cont), CONS(expr, values));
    else if (is_symbol(expr)) {
	obj_t env = cont_env(cont);
	obj_t bdg = env_lookup(env, expr);
	obj_t val = binding_value(bdg);
	return cv(cont_cont(cont), CONS(val, values));
    } else if (is_application(expr)) {
	obj_t operator = application_operator(expr);
	cont = make_cont4(c_eval_operator,
			  cont_cont(cont),
			  cont_env(cont),
			  expr);
	return cv(make_cont4(c_eval, cont, cont_env(cont), operator), values);
    }
    raise(&syntax, expr, "must be expression");
}

NORETURN static void handle_lowex(lowex_type_t type, obj_t ex) 
{
    switch (type) {

    case LT_THROWN:
    case LT_HEAP_FULL:
	longjmp(eval_restart, type);

    case LT_SIGNALLED:
	longjmp(eval_sigrestart, type);

    default:
	ASSERT(false);
    }
}

extern obj_t core_eval(obj_t expr, obj_t env)
{
    obj_t cont     = make_cont4(c_eval, EMPTY_LIST, env, expr);
    obj_t values   = EMPTY_LIST;
    //cv_t registers = { cont, values };

    int j0 = sigsetjmp(eval_sigrestart, 1);
    if (j0 == LT_SIGNALLED) {
	/* push exception... */
    } else {
	ASSERT(j0 == LT_NO_EXCEPTION);
	int j1 = setjmp(eval_restart);
	if (j1 == LT_THROWN) {
	    /* push exception... */
	} else if (j1 == LT_HEAP_FULL) {
	    cont_root           = cont;
	    values_root         = values;
	    collect_garbage();
	    cont   = cont_root;
	    values = values_root;
	} else {
	    ASSERT(j1 == LT_NO_EXCEPTION);
	    register_lowex_handler(handle_lowex);
	}
    }
    while (!is_null(cont)) {
	cv_t ret = cont_proc(cont)(cont, values);
	cont   = ret.cv_cont;
	values = ret.cv_values;
	//oprintf("\t\tvalues=%O\n", values);
    }
    deregister_lowex_handler(handle_lowex);
    ASSERT(is_null(CDR(values)));
    return CAR(values);			/* XXX */
}
