#include "env.h"
#include "prim.h"
#include "list.h"
#include "test.h"
#include "types.h"

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

DEFINE_SPECIAL_FORM(L"lambda")(obj_t cont, obj_t values)
{
    obj_t expr    = cont4_arg(cont);
    obj_t formals = CADR(expr);
    obj_t body    = CDDR(expr);
    return cv(cont_cont(cont),
	      CONS(make_procedure(body, formals, cont_env(cont)), CDR(values)));
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

TEST_EVAL(L"(if (= 0 0) 1 2)",		L"1");
TEST_EVAL(L"(if (= 0 1) 1 2)",		L"2");
TEST_EVAL(L"(if (= 0 0) 1)",		L"1");
TEST_EVAL(L"(if (= 0 1) 1)",		UNDEFINED_REPR);

/* from r6rs */
TEST_EVAL(L"(if (> 3 2) 'yes 'no)",	L"yes");
TEST_EVAL(L"(if (> 2 3) 'yes 'no)",	L"no");
TEST_EVAL(L"(if (> 3 2)\n"
          L"    (- 3 2)\n"
          L"    (+ 3 2))",		L"1");
TEST_EVAL(L"(if #f #f)",		UNDEFINED_REPR);

static cv_t c_continue_set(obj_t cont, obj_t values)
{
    obj_t p  = cont4_arg(cont);
    obj_t env   = cont_env(cont);
    obj_t var   = CAR(p);
    obj_t value = CAR(values);
    obj_t bdg   = env_lookup(env, var);
    EVAL_LOG("var=%O value=%O", var, value);
    /* N.B., allocate values list before mutating environment. */
    obj_t new_values = CONS(UNDEFINED_OBJ, CDR(p));
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
    obj_t second = make_cont4(c_continue_set,
			      cont_cont(cont),
			      env,
			      CONS(CADR(form), CDR(values)));
    obj_t first = make_cont4(c_eval, second, env, expr);
    return cv(first, EMPTY_LIST);
}

TEST_EVAL(L"(define v1 '()) (set! v1 4) v1", L"4");
TEST_EVAL(L"(define v2 '()) (set! v2 4)",    UNDEFINED_REPR);
TEST_EVAL(L"((lambda (x) (set! x #t) x) #f)", L"#t");

/* from r6rs */
//TEST_EVAL(L"(let ((x 2))\n"
//          L"  (+ x 1)\n"
//          L"  (set! x 4)\n"
//          L"  (+ x 1))",        L"5");
