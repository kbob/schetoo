#include "env.h"
#include "prim.h"
#include "list.h"
#include "test.h"
#include "types.h"

static cv_t setq();			/* set! */

DEFINE_COOKED_SPECIAL_FORM(L"quote", 1)(obj_t datum)
{
    return datum;
}

TEST_EVAL(L"(quote ())",		L"()");
TEST_EVAL(L"(quote quote)",		L"quote");
TEST_EVAL(L"(quote (a b c))",		L"(a b c)");

/* from r6rs */
TEST_EVAL(L"(quote a)",			L"a");
TEST_EVAL(L"(quote #(a b c))",		L"#(a b c)");
TEST_EVAL(L"(quote (+ 1 2))",		L"(+ 1 2)");

TEST_EVAL(L"'\"abc\"",			L"\"abc\"");
TEST_EVAL(L"'145932",			L"145932");
TEST_EVAL(L"'a",			L"a");
TEST_EVAL(L"'#(a b c)",			L"#(a b c)");
TEST_EVAL(L"'()",			L"()");
TEST_EVAL(L"'(+ 1 2)",			L"(+ 1 2)");
TEST_EVAL(L"'(quote a)",		L"(quote a)");
TEST_EVAL(L"''a",			L"(quote a)");

#if 0 && !OLD_ENV

#include "oprintf.h"

static cv_t lambda();

static obj_t mogrify_local(obj_t sym)
{
    return sym;
}

static obj_t mogrify_symbol_(obj_t sym, obj_t arglist, obj_t env)
{
    while (is_pair(arglist)) {
	if (sym == CAR(arglist))
	    return mogrify_local(sym);
	arglist = CDR(arglist);
    }
    if (sym == arglist)
	return mogrify_local(sym);
    return env_make_ref(env, sym);
}

static obj_t mogrify_symbol(obj_t sym, obj_t arglist, obj_t env)
{
    oprintf("mogrify_symbol(%O)\n", sym);
    obj_t m = mogrify_symbol_(sym, arglist, env);
    oprintf("mogrify_symbol(%O) => %O\n", sym, m);
    return m;
}

static bool pair_is_lambda(obj_t pair, obj_t env)
{
    obj_t operator = CAR(pair);
    if (is_symbol(operator)) {
	obj_t value = env_lookup(env, operator);
	return is_procedure(value) &&
	       (cont_proc_t)procedure_code(value) == lambda;
    }
    return false;
}

static obj_t mogrify(obj_t expr, obj_t arglist, obj_t env);
static obj_t mogrify_(obj_t expr, obj_t arglist, obj_t env)
{
    // var -> env-ref
    if (is_symbol(expr)) {
	return mogrify_symbol(expr, arglist, env);
    }
    if (is_pair(expr)) {
	if (pair_is_lambda(expr, env)) {
	    return CONS(mogrify(CAR(expr), arglist, env),
			CONS(CADR(expr), 
			     mogrify(CDDR(expr), arglist, env)));
	} else {
	    obj_t a = mogrify(pair_car(expr), arglist, env);
	    obj_t d = mogrify(pair_cdr(expr), arglist, env);
	    return cons_if_changed(expr, a, d);
	}
    }
    return expr;
}

static obj_t mogrify(obj_t expr, obj_t arglist, obj_t env)
{
    oprintf("mogrify(%O)\n", expr);
    obj_t m = mogrify_(expr, arglist, env);
    oprintf("mogrify(%O) => %O\n", expr, m);
    return m;
}
#endif

DEFINE_STATIC_SPECIAL_FORM(lambda, L"lambda")(obj_t cont, obj_t values)
{
    assert(is_cont4(cont));
    obj_t expr    = cont4_arg(cont);
    obj_t formals = CADR(expr);
    obj_t env     = cont_env(cont);
    obj_t body    = CDDR(expr);
#if 0 && !OLD_ENV
    body = mogrify(body, formals, env);
#endif
    return cv(cont_cont(cont),
	      CONS(make_procedure(body, formals, env), CDR(values)));
}

TEST_EVAL(L"((lambda (x) (+ x 3)) 4)",		L"7");
TEST_EVAL(L"((lambda (x) (+ x x)) 4)",		L"8");
TEST_EVAL(L"((lambda (x) (+) (+ x 3)) 4)",	L"7");
TEST_EVAL(L"((lambda (x) (+ x 3) (+)) 4)",	L"0");

/* from r6rs */
TEST_EVAL(L"(lambda (x) (+ x x))",		L"#<proc-lambda>");
TEST_EVAL(L"((lambda (x) (+ x x)) 4)",		L"8");
//TEST_EVAL(L"((lambda (x)\n"
//	  L"   (define (p y)\n"
//	  L"     (+ y 1))\n"
//	  L"   (+ (p x) x))\n"
//	  L" 5)",				L"11");
TEST_EVAL(L"(define reverse-subtract\n"
	  L"  (lambda (x y) (- y x)))\n"
	  L"(reverse-subtract 7 10)",		L"3");
//TEST_EVAL(L"(define add4\n"
//	  L"  (let ((x 4))\n"
//	  L"    (lambda (y) (+ x y))))\n"
//	  L"(add4 6)",				L"10");


TEST_EVAL(L"((lambda x x) 3 4 5 6)",		L"(3 4 5 6)");
TEST_EVAL(L"((lambda (x y . z) z)\n"
          L" 3 4 5 6)",				L"(5 6)");

static cv_t c_continue_if(obj_t cont, obj_t values)
{
    assert(is_cont4(cont));
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
	return cv(cont_cont(cont), CONS(make_unspecified(), CDR(values)));
    else {
	obj_t alternate = CADDDR(form);
	return cv(make_cont4(c_eval, cont_cont(cont), env, alternate),
		  CDR(values));
    }
}

DEFINE_SPECIAL_FORM(L"if")(obj_t cont, obj_t values)
{
    assert(is_cont4(cont));
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

TEST_EVAL(L"(if (= 0 0) 1 2)",		L"1");
TEST_EVAL(L"(if (= 0 1) 1 2)",		L"2");
TEST_EVAL(L"(if (= 0 0) 1)",		L"1");
TEST_EVAL(L"(if (= 0 1) 1)",		UNSPECIFIED_REPR);

/* from r6rs */
TEST_EVAL(L"(if (> 3 2) 'yes 'no)",	L"yes");
TEST_EVAL(L"(if (> 2 3) 'yes 'no)",	L"no");
TEST_EVAL(L"(if (> 3 2)\n"
          L"    (- 3 2)\n"
          L"    (+ 3 2))",		L"1");
TEST_EVAL(L"(if #f #f)",		UNSPECIFIED_REPR);

static cv_t c_continue_set(obj_t cont, obj_t values)
{
    assert(is_cont5(cont));
    obj_t env   = cont_env(cont);
    obj_t var   = cont5_arg1(cont);
    obj_t value = CAR(values);
    EVAL_LOG("var=%O value=%O", var, value);
    /* N.B., allocate values list before mutating environment. */
    obj_t new_values = CONS(make_unspecified(), cont5_arg2(cont));
    obj_t ret = cont_cont(cont);
    env_set(env, var, value);
    return cv(ret, new_values);
}

DEFINE_STATIC_SPECIAL_FORM(setq, L"set!")(obj_t cont, obj_t values)
{
    assert(is_cont4(cont));
    obj_t form = cont4_arg(cont);
    obj_t env = cont_env(cont);
    EVAL_LOG("form=%O", form);
    CHECK(list_length(form) == 3, "set! takes 2 arguments");
    obj_t expr = CADDR(form);
    obj_t second = make_cont5(c_continue_set,
			      cont_cont(cont),
			      env,
			      CADR(form),
			      CDR(values));
    obj_t first = make_cont4(c_eval, second, env, expr);
    return cv(first, EMPTY_LIST);
}

TEST_EVAL(L"(define v1 '()) (set! v1 4) v1", L"4");
TEST_EVAL(L"(define v2 '()) (set! v2 4)",    UNSPECIFIED_REPR);
TEST_EVAL(L"((lambda (x) (set! x #t) x) #f)", L"#t");

/* from r6rs */
//TEST_EVAL(L"(let ((x 2))\n"
//          L"  (+ x 1)\n"
//          L"  (set! x 4)\n"
//          L"  (+ x 1))",        L"5");
