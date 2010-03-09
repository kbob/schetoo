#include "list.h"
#include "prim.h"
#include "test.h"
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

TEST_EVAL(L"(apply + '())",		L"0");
TEST_EVAL(L"(apply + '(2))",		L"2");
TEST_EVAL(L"(apply + '(2 3))",		L"5");
TEST_EVAL(L"(apply + 2 '())",		L"2");
TEST_EVAL(L"(apply + 2 '(3 4))",	L"9");
TEST_EVAL(L"(apply + 2 '(3 4))",	L"9");
TEST_EVAL(L"(apply + 2 3 '(4 5))",	L"14");
TEST_EVAL(L"(define compose\n"
	  L"  (lambda (f g)\n"
	  L"    (lambda args\n"
	  L"      (f (apply g args)))))\n"
	  L"((compose - *) 12 75)",	L"-900");

/* from r6rs */
TEST_EVAL(L"(apply + (list 3 4))",		L"7");
//TEST_EVAL(L"((define compose\n"
//	  L"  (lambda (f g)\n"
//	  L"    lambda args\n"
//	  L"      (f (apply g args)))))\n"
//	  L"((compose sqrt *) 12 75)",	L"30");

static cv_t escape_callcc(obj_t cont, obj_t values)
{
    assert(is_cont4(cont));
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

//TEST_EVAL(L"(call-with-current-continuation\n"
//          L"  (lambda (exit)\n"
//          L"    (for-each (lambda (x)\n"
//          L"                (if (negative? x)\n"
//          L"                    (exit x)))\n"
//          L"              '(54 0 37 -3 245 19))\n"
//          L"    #t))", L"-3");

//TEST_EVAL(L"(define list-length\n"
//          L"  (lambda (obj)\n"
//          L"    (call-with-current-continuation\n"
//          L"      (lambda (return)\n"
//          L"        (letrec ((r\n"
//          L"                  (lambda (obj)\n"
//          L"                    (cond ((null? obj) 0)\n"
//          L"                          ((pair? obj)\n"
//          L"                           (+ (r (cdr obj)) 1))\n"
//          L"                          (else (return #f))))))\n"
//          L"          (r obj))))))\n"
//          L"(list-length '(1 2 3 4))", L"4");

//TEST_EVAL(L"(list-length '(a b . c))", L"#f");

TEST_EVAL(L"(call-with-current-continuation procedure?)", L"#t");

TEST_EVAL(L"(define plus3 '())\n"
          L"(+ 3 (call/cc\n"
          L"      (lambda (exit)\n"
          L"        (set! plus3 exit)\n"
          L"        4)))\n"
          L"(plus3 5)",			L"8");

TEST_EVAL(L"(define plus3 '())\n"
          L"(+ 3 (call/cc\n"
          L"      (lambda (exit)\n"
          L"        (set! plus3 exit)\n"
          L"        4)))\n"
          L"(* 4 (plus3 5))",		L"8");

TEST_EVAL(L"(define call-negative\n"
          L"  (lambda (proc list)\n"
          L"    (if (null? list)\n"
          L"	    #f\n"
          L"        ((lambda (n)\n"
          L"           (if (< n 0)\n"
          L"               (proc n))\n"
          L"           (call-negative proc\n"
          L"                          (cdr list)))\n"
          L"         (car list)))))\n"
          L"(call-with-current-continuation\n"
          L"  (lambda (exit)\n"
          L"    (call-negative exit '(54 0 37 -3 245 19))))", L"-3")
