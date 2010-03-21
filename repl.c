#include "repl.h"

#include <stdio.h>			/* XXX */

#include "env.h"
#include "eval.h"
#include "io.h"				/* XXX */
#include "obj_eof.h"
#include "prim.h"
#include "print.h"			/* XXX */
#include "read.h"

/*
 * ((lambda ()
 *  (define env (draft-environment))
 *  (define (repl)
 *   ((lambda (x)
 *      (if (not (eof-object? x))
 *	  ((lambda ()
 *	     (draft-print (eval x env))
 *	     (repl)))))
 *    (draft-read)))
 *  (repl)))
 */

static cv_t c_print(obj_t cont, obj_t values)
{
    EVAL_LOG("values=%O", values);
    outstream_t *out = make_file_outstream(stdout);
    print(CAR(values), out);
    delete_outstream(out);
    return cv(cont_cont(cont), CDR(values));
}

static cv_t c_test_eof(obj_t cont, obj_t values)
{
    EVAL_LOG("values=%O", values);
    obj_t expr = CAR(values);
    if (is_eof(expr))
	return cv(cont_cont(cont), values);
    obj_t env = cont_env(cont);
    obj_t fourth = make_cont3(c_test_eof, cont_cont(cont), env);
    obj_t third  = make_cont3(c_read, fourth, env);
    obj_t second = make_cont3(c_print, third, env);
    obj_t first  = make_cont4(c_eval, second, env, expr);
    return cv(first, CDR(values));
}

void core_repl(void)
{
    obj_t second = make_cont3(c_test_eof, EMPTY_LIST, root_environment());
    obj_t first = make_cont3(c_read, second, root_environment());
    core_eval_cont(first);
}
