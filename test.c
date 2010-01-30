#include "test.h"

#ifdef NOTEST

void self_test()
{}

#else

#define TEST_TRACE 0

#include <string.h>
#include <stdlib.h>

#include "eval.h"
#include "except.h"
#include "heap.h"
#include "io.h"
#include "low_ex.h"
#include "print.h"
#include "read.h"
#include "roots.h"

typedef int (*test_driver_t)(const test_case_descriptor_t *);

static jmp_buf test_restart;
static test_case_descriptor_t *test_case_descriptors;

static char *phase_name(test_phase_t phase)
{
    switch (phase) {
    case TP_READ: return "read";
    case TP_EVAL: return "eval";
    default:      ASSERT(0);
		  return NULL;
    }
}

static void handle_lowex(lowex_type_t type, obj_t ex) NORETURN;
static void handle_lowex(lowex_type_t type, obj_t ex) 
{
    switch (type) {

    case LT_THROWN:
    case LT_HEAP_FULL:
	longjmp(test_restart, type);

    case LT_SIGNALLED:
	ASSERT(false);

    default:
	ASSERT(false);
    }
}

static int read_driver(const test_case_descriptor_t *tc)
{
    int err_count = 0;
#if TEST_TRACE
    printf("%s:%d read %ls\n", tc->tcd_file, tc->tcd_lineno, tc->tcd_input);
#endif
    /*
    static int counter;
    if (++counter < 147) {
	return 0;
    }
    printf("counter = %d\n", counter);
    */
    if (tc->tcd_expected == (wchar_t *)&lexical) {
	return err_count;	    /* XXX exceptions unimplemented */
    }
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
	ASSERT(ok);
	break;

    default:
	ASSERT(false);
    }
    deregister_lowex_handler(handle_lowex);
    delete_instream(in);
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
	printf("    expected_len = %d\n", tc->tcd_expected_len);
	printf("\n");
	err_count++;
    }
    return err_count;
}

#if 0
static int eval_driver(const test_case_descriptor_t *tc)
{
    int err_count = 0;
#if TEST_TRACE
    printf("%s:%d eval %ls\n", tc->tcd_file, tc->tcd_lineno, tc->tcd_input);
#endif
    instream_t *in =
	make_string_instream(tc->tcd_input, wcslen(tc->tcd_input));
    AUTO_ROOT(expr, NIL);
    AUTO_ROOT(value, NIL);
    AUTO_ROOT(env, make_env(library_env(r6rs_library())));
    while (read_stream(in, &expr))
	value = eval(expr, env);
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
    POP_FUNCTION_ROOTS();
    return err_count;
}
#endif

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
    // test_all(TP_EVAL, eval_driver);
}

#endif /* !NOTEST */
