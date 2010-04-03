#include "test.h"

#ifdef NOTEST

void self_test()
{}

#else

#define TEST_TRACE 1

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "env.h"
#include "eval.h"
#include "except.h"
#include "heap.h"
#include "io.h"
#include "list.h"
#include "low_ex.h"
#include "obj_cont.h"
#include "obj_record.h"
#include "obj_rtd.h"
#include "obj_string.h"
#include "obj_symbol.h"
#include "obj_vector.h"
#include "oprintf.h"
#include "prim.h"
#include "print.h"
#include "read.h"

typedef int (*test_driver_t)(const test_case_descriptor_t *);

#if 0
static obj_t test_ex;
static jmp_buf test_restart;
#endif
static test_case_descriptor_t *test_case_descriptors;

static char *phase_name(test_phase_t phase)
{
    switch (phase) {
    case TP_READ: return "read";
    case TP_EVAL: return "eval";
    default:      assert(0);
		  return NULL;
    }
}

#if 0

NORETURN static void handle_lowex(lowex_type_t type, obj_t ex) 
{
    switch (type) {

    case LT_THROWN:
	test_ex = ex;
	/* fall through */
    case LT_HEAP_FULL:
	longjmp(test_restart, type);

    case LT_SIGNALLED:
	assert(false);

    default:
	assert(false);
    }
}

#else

static cv_t c_test_handler(obj_t cont, obj_t values)
{
    obj_t ex = vector_ref(record_get_field(CAR(values), 0), 0);
    return cv(EMPTY_LIST, MAKE_LIST(ex));
}

#endif

static int read_driver(const test_case_descriptor_t *tc)
{
    int err_count = 0;
#if TEST_TRACE
    printf("%s:%d read %ls\n", tc->tcd_file, tc->tcd_lineno, tc->tcd_input);
#endif
#if 0
    instream_t *in = NULL;
    obj_t obj;
    bool ok;
    register_lowex_handler(handle_lowex);
    switch (setjmp(test_restart)) {

    case LT_HEAP_FULL:
	delete_instream(in);
	collect_garbage();
	/* fall through */

    case LT_NO_EXCEPTION:
	in = make_string_instream(tc->tcd_input, wcslen(tc->tcd_input));
	ok = read_stream(in, &obj);
	ok = ok;
	assert(ok);
	break;

    case LT_THROWN:
	obj = rtd_name(record_rtd(vector_ref(record_get_field(test_ex, 0), 0)));
	break;

    default:
	assert(false);
    }
    deregister_lowex_handler(handle_lowex);
    delete_instream(in);
#elif 0
    obj_t input = make_string_from_C_str(tc->tcd_input);
    obj_t osip_sym = make_symbol_from_C_str(L"open-string-input-port");
    obj_t read_sym = make_symbol_from_C_str(L"read");
    obj_t expr = MAKE_LIST(read_sym, MAKE_LIST(osip_sym, input));
    obj_t obj = core_eval(expr, root_environment());
#else
    obj_t input    = make_string_from_C_str(tc->tcd_input);
    obj_t osip_sym = make_symbol_from_C_str(L"open-string-input-port");
    obj_t read_sym = make_symbol_from_C_str(L"read");
    obj_t expr     = MAKE_LIST(read_sym, MAKE_LIST(osip_sym, input));
    obj_t env      = root_environment();
    obj_t cont     = make_cont4(c_eval, EMPTY_LIST, env, expr);
    obj_t hname    = make_symbol_from_C_str(L"test-handler");
    obj_t handler  = make_raw_procedure(c_test_handler, hname, env);
    obj_t obj      = core_eval_cont(cont, EMPTY_LIST, handler);
#endif
    const size_t out_size = 100;
    wchar_t actual[out_size + 1];
    outstream_t *out = make_string_outstream(actual, out_size);
    princ(obj, out);
    size_t expected_bytes = sizeof (wchar_t) * (tc->tcd_expected_len + 1);
    if (memcmp(actual, tc->tcd_expected, expected_bytes)) {
	int i;

	printf("%s:%d FAIL read test\n", tc->tcd_file, tc->tcd_lineno);
	printf("    input        = %ls\n", tc->tcd_input);
        printf("    actual       = %ls = {", actual);
        for (i = 0; i <= tc->tcd_expected_len; i++)
	    printf("\\x%02x, ", actual[i]);
        printf("};\n");
	printf("    expected     = %ls = {", tc->tcd_expected);
        for (i = 0; i <= tc->tcd_expected_len; i++)
	    printf("\\x%02x, ", tc->tcd_expected[i]);
        printf("};\n");
	printf("    expected_len = %zd", tc->tcd_expected_len);
	printf("\n");
	err_count++;
    }
    return err_count;
}

/*
 * (define (run-test str)
 *   (let ((port (open-input-string-port str))
 *	   (env (copy-environment (root-environment)))
 *	   (read-all (lambda (last)
 *		       (let ((form (read port)))
 *			 (if (eof-object? form)
 *			     last
 *			   (read-all (eval form env)))))))
 *     (read-all #f)))
 *
 *  ((lambda (str env)
 *
 */
			  

static int eval_driver(const test_case_descriptor_t *tc)
{
    int err_count = 0;
#if TEST_TRACE
    printf("%s:%d eval %ls\n", tc->tcd_file, tc->tcd_lineno, tc->tcd_input);
#endif
#if 0
    instream_t *in =
	make_string_instream(tc->tcd_input, wcslen(tc->tcd_input));
    obj_t expr = UNDEFINED_OBJ;
    obj_t value = EMPTY_LIST;
    obj_t env = make_env(root_environment());
    while (read_stream(in, &expr))
	value = core_eval(expr, env);
#else
    static const char_t test_source[] =
	L"(lambda (port loop env)					\n"
	L"   (set! loop (lambda (form last)				\n"
        L"                (if (eof-object? form)			\n"
        L"                    last					\n"
        L"                   (loop (read port) (eval form env)))))	\n"
	L"   (loop (read port) #f))";
#define P(x) (oprintf(#x "=%O\n", (x)))
    obj_t root_env  = root_environment();
    obj_t tsrc_str  = make_string_from_C_str(test_source);
    obj_t eval_sym  = make_symbol_from_C_str(L"eval");
    obj_t read_sym  = make_symbol_from_C_str(L"read");
    obj_t osip_sym  = make_symbol_from_C_str(L"open-string-input-port");
    obj_t renv_sym  = make_symbol_from_C_str(L"root-environment");
    /*
     * (eval (read (open-string-input-port "..."))
     *       (root-environment))
     */
    obj_t form1     = MAKE_LIST(eval_sym,
				MAKE_LIST(read_sym,
					  MAKE_LIST(osip_sym, tsrc_str)),
				MAKE_LIST(renv_sym));
    obj_t test_proc = core_eval(form1, root_env);
    obj_t input_str = make_string_from_C_str(tc->tcd_input);
    obj_t port      = core_eval(MAKE_LIST(osip_sym, input_str), root_env);
    obj_t test_env  = make_env(root_env);
    obj_t test_args = MAKE_LIST(test_env, FALSE_OBJ, port);
    obj_t cont      = make_cont5(c_apply_proc,
				 EMPTY_LIST,
				 root_env,
				 test_proc,
				 EMPTY_LIST);
    obj_t hname     = make_symbol_from_C_str(L"test-handler");
    obj_t handler   = make_raw_procedure(c_test_handler, hname, root_env);
    obj_t value     = core_eval_cont(cont, test_args, handler);
    P(value);
#endif
    /* Compare the value of the last expression. */
    const size_t out_size = 100;
    wchar_t actual[out_size + 1];
    outstream_t *out = make_string_outstream(actual, out_size);
    princ(value, out);
    delete_outstream(out);
    if (wcscmp(actual, tc->tcd_expected)) {
	printf("%s:%d FAIL eval test\n", tc->tcd_file, tc->tcd_lineno);
	printf("    input    = %ls\n", tc->tcd_input);
        printf("    actual   = %ls\n", actual);
	printf("    expected = %ls\n", tc->tcd_expected);
	printf("\n");
	err_count++;
    }
    return err_count;
}

static void test_all(test_phase_t phase, test_driver_t driver)
{
    int err_count = 0;
    int test_count = 0;
    test_case_descriptor_t *tc;
    for (tc = test_case_descriptors; tc; tc = tc->tcd_next)
	if (tc->tcd_phase == phase) {
	    test_count++;
	    err_count += (*driver)(tc);
	}
    if (err_count) {
	fprintf(stderr, "%d error%s in phase %s.  FAIL.\n",
		err_count, &"s"[err_count == 1], phase_name(phase));
	exit(1);
    } else
	printf("  %3d %s test%s passed.\n",
	       test_count, phase_name(phase), &"s"[test_count == 1]);
}

void register_test(test_case_descriptor_t *tc)
{
    tc->tcd_next = test_case_descriptors;
    test_case_descriptors = tc;
}

void self_test()
{
    test_all(TP_READ, read_driver);
    test_all(TP_EVAL, eval_driver);
}

#endif /* !NOTEST */
