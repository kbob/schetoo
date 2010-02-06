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

static __thread sigjmp_buf eval_sigrestart;
static __thread jmp_buf    eval_restart;

THREAD_ROOT(cont_root);
THREAD_ROOT(values_root);
THREAD_ROOT(env_root);

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

static obj_t c_eval(obj_t cont, obj_t *p_values, obj_t *p_env);

static obj_t c_apply_procedure(obj_t cont, obj_t *p_values, obj_t *p_env)
{
    ASSERT(is_cont3(cont));
    obj_t stop_values = cont3_arg(cont);
    //oprintf("c_apply_procedure stop_values = %O\n", stop_values);
    obj_t p = *p_values;
    obj_t arg_list = EMPTY_LIST;
    size_t arg_count = 0;
    while (p != stop_values) {
	arg_list = CONS(CAR(p), arg_list);
	p = CDR(p);
	arg_count++;
    }
    obj_t operator = CAR(p);
    *p_values = CONS(apply_proc(operator, arg_list, arg_count), CDR(p));
    return cont_cont(cont);
}

static obj_t c_eval_operands(obj_t cont, obj_t *p_values, obj_t *p_env)
{
    ASSERT(is_cont3(cont));
    obj_t appl = cont3_arg(cont);
    obj_t operator = CAR(*p_values);
    //oprintf("c_eval_operands appl = %O; operator = %O\n", appl, operator);
    if (!is_procedure(operator)) {
	raise(&syntax, NULL, "must be procedure", operator);
    }
    if (procedure_is_special_form(operator)) {
	ASSERT(false && "implement me");
    }
    obj_t arg_list = reverse_list(application_operands(appl));
    cont = make_cont3(c_apply_procedure, cont_cont(cont), *p_values);
    while (!is_null(arg_list)) {
	cont = make_cont3(c_eval, cont, CAR(arg_list));
	arg_list = CDR(arg_list);
    }
    return cont;
}

static obj_t c_eval(obj_t cont, obj_t *p_values, obj_t *p_env)
{
    ASSERT(is_cont3(cont));
    obj_t expr = cont3_arg(cont);
    //oprintf("c_eval expr=%O\n", expr);
    if (is_self_evaluating(expr)) {
	*p_values = CONS(expr, *p_values);
	return cont_cont(cont);
    } else if (is_symbol(expr)) {
	*p_values = CONS(binding_value(env_lookup(*p_env, expr)), *p_values);
	return cont_cont(cont);
    } else if (is_application(expr)) {
	obj_t operator = application_operator(expr);
	cont = make_cont3(c_eval_operands, cont_cont(cont), expr);
	return make_cont3(c_eval, cont, operator);
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
    obj_t cont = make_cont3(c_eval, EMPTY_LIST, expr);
    obj_t values = EMPTY_LIST;
    int j0 = sigsetjmp(eval_sigrestart, 1);
    if (j0 == LT_SIGNALLED) {
	printf("eval: SIGNALLED\n");
	/* push exception... */
    }
    int j1 = setjmp(eval_restart);
    if (j1 == LT_THROWN) {
	printf("eval: THROWN\n");
	/* push exception... */
    } else if (j1 == LT_HEAP_FULL) {
	cont_root   = expr;
	values_root = values;
	env_root    = env;
	collect_garbage();
	cont        = cont_root;
	values      = values_root;
	env         = env_root;
    }
    register_lowex_handler(handle_lowex);
    while (!is_null(cont)) {
	cont = cont_proc(cont)(cont, &values, &env);
    }
    deregister_lowex_handler(handle_lowex);
    return CAR(values);			/* XXX */
}
