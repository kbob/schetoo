#include "eval.h"

#include <setjmp.h>
#include <stdio.h>

#include "env.h"
#include "except.h"
#include "heap.h"
#include "low_ex.h"
#include "obj_cont.h"
#include "roots.h"
#include "types.h"

static __thread sigjmp_buf eval_sigrestart;
static __thread jmp_buf    eval_restart;

THREAD_ROOT(cont_root);
THREAD_ROOT(values_root);
THREAD_ROOT(env_root);

typedef obj_t (*C_procedure_t)(obj_t);

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

static obj_t c_eval(obj_t cont, obj_t *p_values, obj_t *p_env)
{
    obj_t expr = continuation3_arg(cont);
    if (is_self_evaluating(expr)) {
	*p_values = CONS(expr, *p_values);
	return continuation_cont(cont);
    } else if (is_symbol(expr)) {
	*p_values = CONS(binding_value(env_lookup(*p_env, expr)), *p_values);
	return continuation_cont(cont);
    } else if (is_application(expr)) {
	ASSERT(false && "implement me");
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
    obj_t cont = make_continuation3(c_eval, EMPTY_LIST, expr);
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
    while (cont != EMPTY_LIST) {
	cont = continuation_proc(cont)(cont, &values, &env);
    }
    deregister_lowex_handler(handle_lowex);
    return CAR(values);			/* XXX */
}
