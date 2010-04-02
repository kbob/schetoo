#include "repl.h"

#include <stdio.h>			/* XXX */
#include <string.h>

#include "env.h"
#include "eval.h"
#include "io.h"				/* XXX */
#include "obj_eof.h"
#include "obj_record.h"
#include "obj_string.h"
#include "obj_symbol.h"
#include "obj_vector.h"
#include "oprintf.h"
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

static cv_t c_handler(obj_t cont, obj_t values)
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
	    ofprintf(stderr, "%*s: %O\n", psnl, psn, rtd_name(rtd));
	    psn = "";
	}
    }
    obj_t who_p = FALSE_OBJ;
    obj_t irr_p = UNDEFINED_OBJ;
    for (i = 0; i < size; i++) {
	obj_t p = vector_ref(parts, i);
	obj_t rtd = record_rtd(p);
	if (rtd == message) {
	    obj_t msg = record_get_field(p, 0);
	    const wchar_t *chars = string_value(msg);
	    fprintf(stderr, "%*s  %ls\n", psnl, psn, chars);
	    psn = "";
	} else if (rtd == who)
	    who_p = record_get_field(p, 0);
	else if (rtd == irritants)
	    irr_p = record_get_field(p, 0);
    }
    if (who_p != FALSE_OBJ && irr_p != UNDEFINED_OBJ) {
	ofprintf(stderr, "%*s  %O\n", psnl, psn, CONS(who_p, irr_p));
	psn = "";
    } else if (who_p != FALSE_OBJ) {
	ofprintf(stderr, "%*s  %O\n", psnl, psn, who_p);
	psn = "";
    } else if (irr_p != UNDEFINED_OBJ) {
	ofprintf(stderr, "%*s  %O\n", psnl, psn, irr_p);
	psn = "";
    }
    if (*psn)
	fprintf(stderr, "%s: unknown exception\n", psn);
    ofprintf(stderr, "\n");
    obj_t root_env = root_environment();
    obj_t second   = make_cont3(c_test_eof, EMPTY_LIST, root_env);
    obj_t first    = make_cont3(c_read, second, root_env);
    return cv(first, EMPTY_LIST);
}

void core_repl(void)
{
    obj_t root_env = root_environment();
    obj_t hname    = make_symbol_from_C_str(L"default-exception-handler");
    obj_t handler  = make_raw_procedure(c_handler, hname, root_env);
    obj_t second   = make_cont3(c_test_eof, EMPTY_LIST, root_env);
    obj_t first    = make_cont3(c_read, second, root_env);
    core_eval_cont(first, handler);
}

#if 0

#include "list.h"

void core_repl(void)
{
    obj_t env = root_environment();
    oprintf("%s:%d env=%p\n", __FILE__, __LINE__, env);
    obj_t osip_sym = make_symbol_from_C_str(L"open-string-input-port");
    obj_t read_sym = make_symbol_from_C_str(L"read");

    //obj_t str = make_string_from_C_str(L"asdf");
    //obj_t str = make_string_from_C_str(L"()");
    obj_t str = make_string_from_C_str(L"(+ 3 4)");
    obj_t form = MAKE_LIST(read_sym, MAKE_LIST(osip_sym, str));
    oprintf("form=%O\n", form);
    obj_t result = core_eval(form, env);
    oprintf("%s:%d result=%O\n", __FILE__, __LINE__, result);
}

#endif
