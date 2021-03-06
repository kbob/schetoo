#include "eval.h"

#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "except.h"
#include "heap.h"
#include "list.h"
#include "low_ex.h"
#include "obj_cont.h"
#include "obj_record.h"
#include "oprintf.h"
#include "prim.h"
#include "record.h"
#include "roots.h"
#include "test.h"
#include "transaction.h"
#include "types.h"

/*
 * I'm having trouble figuring out exactly what values is.
 *
 * I think it's a list of computed values in the current argument
 * list.  Except sometimes (e.g., c_eval_operator) it's just the list
 * of the operator.
 * 
 * It could probably be turned into a vector.
 */

DEFINE_STATIC_RECORD_TYPE(dyn_env, L"dynamic-environment", NULL, RF_OPAQUE) = {
    { FM_MUTABLE, L"dynamic-unwind" },
    { FM_MUTABLE, L"exception-handler" },
    { FM_END }
};

typedef enum dyn_env_fields {
    DE_UNWIND,
    DE_HANDLER,
} dyn_env_fields_t;

static __thread sigjmp_buf eval_sigrestart;
static __thread jmp_buf    eval_restart;
static __thread obj_t      eval_exception;

THREAD_ROOT(cont_root);
THREAD_ROOT(values_root);
THREAD_ROOT(eval_dyn_env);

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

static obj_t apply_proc(obj_t proc, obj_t arg_list)
{
    size_t arg_count = list_length(arg_list);
    interval_t ivl = procedure_arg_range(proc);
    int inf = interval_is_infinite(ivl);
    CHECK(arg_count >= interval_lower_bound(ivl), "not enough arguments");
    CHECK(inf || arg_count <= interval_upper_bound(ivl), "too many arguments");
    const size_t max_args = 3;
    size_t i, u;
    if (inf)
	u = interval_lower_bound(ivl);
    else
	u = interval_upper_bound(ivl);
    assert(u + inf <= max_args);
    obj_t a[max_args];
    for (i = 0; i < u; i++) {
	if (is_null(arg_list))
	    a[i] = MISSING_ARG;
	else {
	    a[i] = CAR(arg_list);
	    arg_list = CDR(arg_list);
	}
    }
    if (interval_is_infinite(ivl))
	a[u++] = arg_list;

    C_procedure_t code = procedure_code(proc);
    switch (u) {

    /* N.B.  Call primitive after all other allocations. */
    case 0: return code();
    case 1: return code(a[0]);
    case 2: return code(a[0], a[1]);
    case 3: return code(a[0], a[1], a[2]);

    default:
	assert(false && "implement me");
    }
}

/* N.B., c_eval_seq discards CAR(values) so pass it an extra value. */
static cv_t c_eval_seq(obj_t cont, obj_t values)
{
    assert(is_cont4(cont));
    obj_t env = cont_env(cont);
    obj_t exprs = cont4_arg(cont);
    EVAL_LOG("exprs=%O", exprs);
    obj_t second = cont_cont(cont);
    if (!is_null(CDR(exprs)))
	second = make_cont4(c_eval_seq, second, env, CDR(exprs));
    obj_t first = make_cont4(c_eval, second, env, CAR(exprs));
    return cv(first, CDR(values));
}

extern cv_t c_apply_proc(obj_t cont, obj_t values)
{
    assert(is_cont5(cont));
    obj_t next = cont_cont(cont);
    obj_t operator = cont5_arg1(cont);
    obj_t saved_values = cont5_arg2(cont);
    EVAL_LOG("op=%O values=%O operator=%O saved_values=%O",
	     operator, values, operator, saved_values);
    obj_t arg_list = reverse_list(values);
    if (procedure_is_C(operator)) {
	if (procedure_is_raw(operator))
	    return ((cont_proc_t)procedure_code(operator))(cont, values);
	else {
	    // N.B., call proc after all other allocations.
	    obj_t new_values = CONS(make_uninitialized(), saved_values);
	    pair_set_car_nc(new_values, apply_proc(operator, arg_list));
	    return cv(next, new_values);
	}
    } else {
	obj_t env = procedure_env(operator);
	obj_t formals = procedure_args(operator);
	obj_t actuals = arg_list;
	obj_t new_env = make_closed_env(env, formals, actuals);
	obj_t body    = procedure_body(operator);

	// Push a value for c_eval_seq to discard.
	obj_t new_values = CONS(make_uninitialized(), saved_values);
	return cv(make_cont4(c_eval_seq, next, new_env, body), new_values);
    }
}

static cv_t c_eval_operator(obj_t cont, obj_t values)
{
    assert(is_cont4(cont));
    obj_t appl = cont4_arg(cont);
    obj_t operator = CAR(values);
    EVAL_LOG("appl=%O operator=%O", appl, operator);
    COULD_RETRY();
    if (!is_procedure(operator))
	SYNTAX_ERROR(operator, operator, "must be procedure");
    if (!procedure_args_evaluated(operator)) {
	assert(procedure_is_C(operator) && "implement Scheme special forms");
	if (procedure_is_raw(operator)) {
	    return ((cont_proc_t)procedure_code(operator))(cont, values);
	} else {
	    // N.B., call proc after all other allocations.
	    obj_t arg_list = application_operands(appl);
	    obj_t new_values = CONS(make_uninitialized(), CDR(values));
	    pair_set_car(new_values, apply_proc(operator, arg_list));
	    return cv(cont_cont(cont), new_values);
	}
    }
    obj_t arg_list = reverse_list(application_operands(appl));
    cont = make_cont5(c_apply_proc,
		      cont_cont(cont),
		      cont_env(cont),
		      operator,
		      CDR(values));
    while (!is_null(arg_list)) {
	cont = make_cont4(c_eval, cont, cont_env(cont), CAR(arg_list));
	arg_list = CDR(arg_list);
    }
    return cv(cont, EMPTY_LIST);
}

extern cv_t c_eval(obj_t cont, obj_t values)
{
    assert(is_cont4(cont));
    obj_t expr = cont4_arg(cont);
    EVAL_LOG("expr=%O", expr);
    COULD_RETRY();
    if (is_self_evaluating(expr))
	return cv(cont_cont(cont), CONS(expr, values));
    else if (is_symbol(expr)) {
	obj_t env = cont_env(cont);
	obj_t val = env_lookup(env, expr);
	return cv(cont_cont(cont), CONS(val, values));
#if !OLD_ENV
    } else if (is_env_ref(expr)) {
	return cv(cont_cont(cont),
		  CONS(env_ref_lookup(cont_env(cont), expr), values));
#endif
    } else if (is_application(expr)) {
	obj_t operator = application_operator(expr);
	obj_t env = cont_env(cont);
	obj_t second = make_cont4(c_eval_operator,
				  cont_cont(cont),
				  env,
				  expr);
	obj_t first = make_cont4(c_eval, second, env, operator);
	return cv(first, values);
    }
    SYNTAX_ERROR(expr, expr, "must be expression");
}

NORETURN static void handle_lowex(lowex_type_t type, obj_t ex)
{
    switch (type) {

    case LT_THROWN:
	eval_exception = ex;
	/* FALLTHROUGH */
    case LT_HEAP_FULL:
	longjmp(eval_restart, type);

    case LT_SIGNALLED:
	longjmp(eval_sigrestart, type);

    default:
	assert(false);
    }
    abort();
}

static cv_t default_handler(obj_t cont, obj_t values)
{
    /*
     * For all non-message-irritants-who conditions,
     *     print the condition's class.
     * For all message conditions, print the condition's message.
     * If who or irritants given, print (cons who irritants).
     */

    assert(is_cont(cont));
    EVAL_LOG("cont=%O values=%O", cont, values);
    obj_t ex = CAR(values);
    obj_t parts = record_get_field(ex, 0);
    const char *psn = program_short_name();
    size_t psnl = strlen(psn);
    size_t i, size = vector_len(parts);
    for (i = 0; i < size; i++) {
	obj_t rtd = record_rtd(vector_ref(parts, i));
	if (rtd != message && rtd != irritants && rtd != who) {
	    ofprintf(stderr, "%*s: %O\n", (int)psnl, psn, rtd_name(rtd));
	    psn = "";
	}
    }
    obj_t who_p = FALSE_OBJ;
    obj_t irr_p = make_uninitialized();
    for (i = 0; i < size; i++) {
	obj_t p = vector_ref(parts, i);
	obj_t rtd = record_rtd(p);
	if (rtd == message) {
	    obj_t msg = record_get_field(p, 0);
	    const wchar_t *chars = string_value(msg);
	    fprintf(stderr, "%*s  %ls\n", (int)psnl, psn, chars);
	    psn = "";
	} else if (rtd == who)
	    who_p = record_get_field(p, 0);
	else if (rtd == irritants)
	    irr_p = record_get_field(p, 0);
    }
    if (who_p != FALSE_OBJ && !is_uninitialized(irr_p)) {
	ofprintf(stderr, "%*s  %O\n", (int)psnl, psn, CONS(who_p, irr_p));
	psn = "";
    } else if (who_p != FALSE_OBJ) {
	ofprintf(stderr, "%*s  %O\n", (int)psnl, psn, who_p);
	psn = "";
    } else if (!is_uninitialized(irr_p)) {
	ofprintf(stderr, "%*s  %O\n", (int)psnl, psn, irr_p);
	psn = "";
    }
    if (*psn)
	fprintf(stderr, "%s: unknown exception\n", psn);
    ofprintf(stderr, "\n");
    return cv(EMPTY_LIST, CONS(make_uninitialized(), EMPTY_LIST));
}

static cv_t c_exception_returned(obj_t cont, obj_t values)
{
    assert(is_cont4(cont));
    obj_t handler = cont4_arg(cont);
    EVAL_LOG("handler=%O", handler);
    CHECK_CONDITION(false,
		    &non_continuable, "exception handler returned",
		    handler);
}

static obj_t add_who_irritants(obj_t cont, obj_t values, obj_t ex)
{
    //oprintf("add_who_irritants: cont_proc = %p\n", cont_proc(cont));
    //oprintf("add_who_irritants: cont4_arg = %O\n", cont4_arg(cont));
    //oprintf("add_who_irritants: values = %O\n", values);
    assert(is_cont(cont));
    cont_proc_t proc = cont_proc(cont);
    if (proc == c_apply_proc) {
	assert(is_cont5(cont));
	obj_t op = cont5_arg1(cont);
	obj_t who_sym = procedure_is_C(op) ? procedure_name(op) : FALSE_OBJ;
	obj_t who_ex = MAKE_RECORD(who, who_sym);
	obj_t irr_ex = MAKE_RECORD(irritants, reverse_list(values));
	return MAKE_COMPOUND_CONDITION(who_ex, irr_ex, ex);
    }
    if (proc == c_eval_operator) {
	assert(is_cont4(cont));
	obj_t arg = cont4_arg(cont);
	obj_t who_ex = MAKE_RECORD(who, CAR(arg));
	obj_t irr_ex = MAKE_RECORD(irritants, CDR(arg));
	return MAKE_COMPOUND_CONDITION(who_ex, irr_ex, ex);
    }
    if (proc == c_eval) {
	assert(is_cont4(cont));
	obj_t expr = cont4_arg(cont);
	obj_t who_ex = MAKE_RECORD(who, expr);
	return MAKE_COMPOUND_CONDITION(who_ex, ex);
    }
    return ex;
}

static cv_t push_exception(obj_t cont, obj_t values)
{
    assert(is_cont(cont));
    obj_t ex = add_who_irritants(cont, values, eval_exception);
    obj_t handler = record_get_field(eval_dyn_env, DE_HANDLER);
    obj_t second = make_cont4(c_exception_returned,
			      EMPTY_LIST,
			      cont_env(cont),
			      EMPTY_LIST);
    obj_t first = make_cont5(c_apply_proc,
			     second,
			     cont_env(cont),
			     handler,
			     values);
    return cv(first, CONS(ex, values));
}

extern obj_t core_eval_cont(volatile obj_t cont,
			    volatile obj_t values,
			    obj_t handler)
{
    eval_dyn_env = MAKE_RECORD(dyn_env, EMPTY_LIST, handler);

    if (sigsetjmp(eval_sigrestart, 1)) {
	/* On Linux, siglongjmp is 30X slower than longjmp. */
	/* push exception... */
    } else {
	switch (setjmp(eval_restart)) {

	case LT_THROWN:
	    {
		cv_t ret = push_exception(cont, values);
		cont     = ret.cv_cont;
		values   = ret.cv_values;
	    }
	    break;

	case LT_HEAP_FULL:
	    cont_root   = cont;
	    values_root = values;
	    collect_garbage();
	    cont        = cont_root;
	    values      = values_root;
	    cont_root   = make_uninitialized();
	    values_root = make_uninitialized();
	    break;

	case LT_NO_EXCEPTION:
	    register_lowex_handler(handle_lowex);
	    break;

	default:
	    assert(false);
	}
    }
    while (!is_null(cont)) {
	cv_t ret = cont_proc(cont)(cont, values);
	cont   = ret.cv_cont;
	values = ret.cv_values;
	COMMIT();
#if DEBUG_EVAL
	int n = 0;
	obj_t p;
	for (p = cont; !is_null(p); p = cont_cont(p))
	    n++;
	EVAL_LOG("values=%O cont depth=%d", values, n);
#endif
    }
    deregister_lowex_handler(handle_lowex);
    eval_dyn_env = make_uninitialized();
    EVAL_LOG("END values=%O", values);
    assert(is_null(CDR(values)));
    return CAR(values);
}

extern obj_t core_eval(obj_t expr, obj_t env)
{
    obj_t proc_name = make_symbol_from_C_str(L"default-exception-handler");
    obj_t dhproc =
	make_raw_procedure(default_handler, proc_name, root_environment());
    obj_t cont = make_cont4(c_eval, EMPTY_LIST, env, expr);
    return core_eval_cont(cont, EMPTY_LIST, dhproc);
}

TEST_EVAL(L"#t",				L"#t");
TEST_EVAL(L"#f",				L"#f");
TEST_EVAL(L"-23",				L"-23");
TEST_EVAL(L"#\\a",				L"#\\a");
TEST_EVAL(L"\"foo\"",				L"\"foo\"");

TEST_EVAL(L"+",					L"#<proc-C-+>");
TEST_EVAL(L"((lambda () +))",			L"#<proc-C-+>");
TEST_EVAL(L"((lambda (x) x) #t)",		L"#t");
TEST_EVAL(L"((lambda (a)\n"
	  L"   ((lambda (b)\n"
	  L"      a)\n"
	  L"    2))\n"
	  L" 1)",				L"1");
TEST_EVAL(L"(+ 3 ((lambda (x) x (+ x x)) 5))",	L"13");
TEST_EVAL(L"((lambda (a)\n"
	  L"   (cons\n"
	  L"    ((lambda (a)\n"
	  L"       a)\n"
	  L"     3)\n"
	  L"    a))\n"
	  L" 2)",				L"(3 . 2)");
