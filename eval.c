#include "eval.h"

#include <setjmp.h>
#include <stdio.h>

#include "except.h"
#include "heap.h"
#include "obj_cont.h"
#include "obj_pair.h"
#include "roots.h"

enum RESTART_REASON {
    RR_NO_RESTART = 0,
    RR_SIGNALLED,
    RR_THROWN,
    RR_THROWN_CONTINUABLE,
    RR_HEAP_FULL,
};

static __thread bool       restart_buffers_valid;
static __thread sigjmp_buf eval_sigrestart;
static __thread jmp_buf    eval_restart;

THREAD_ROOT(cont_root);
THREAD_ROOT(values_root);

typedef obj_t (*C_procedure_t)(obj_t);

obj_t EXPR, VALUE;
C_procedure_t CONT;

static inline bool is_self_evaluating(obj_t expr)
{
    return true;
}

static obj_t c_eval(obj_t cont, obj_t *values)
{
    obj_t expr = continuation3_arg(cont);
    if (is_self_evaluating(expr))
	*values = make_pair(expr, *values);
    else
	ASSERT(false);
    return continuation_cont(cont);
}

extern obj_t core_eval(obj_t expr, obj_t env)
{
    obj_t cont = make_continuation3(c_eval, EMPTY_LIST, expr);
    obj_t values = EMPTY_LIST;
    int j0 = sigsetjmp(eval_sigrestart, 1);
    if (j0 == RR_SIGNALLED) {
	printf("eval: SIGNALLED\n");
	/* push exception... */
    }
    int j1 = setjmp(eval_restart);
    if (j1 == RR_THROWN) {
	printf("eval: THROWN\n");
	/* push exception... */
    } else if (j1 == RR_THROWN_CONTINUABLE) {
	printf("eval: THROWN\n");
	/* push exception... */
    } else if (j1 == RR_HEAP_FULL) {
	cont_root = expr;
	values_root = values;
	collect_garbage();
	cont = cont_root;
	values = values_root;
    }
    restart_buffers_valid = true;
    while (cont != EMPTY_LIST) {
	cont = continuation_proc(cont)(cont, &values);
    }
    restart_buffers_valid = false;
    return pair_car(values);
}

void sigrestart(void) /* __attribute__ ((noreturn)) */
{
    ASSERT(restart_buffers_valid);
    siglongjmp(eval_sigrestart, RR_SIGNALLED);
}

void restart(bool continuable) /* __attribute__ ((noreturn)) */
{
    ASSERT(restart_buffers_valid);
    longjmp(eval_restart, continuable ? RR_THROWN_CONTINUABLE : RR_THROWN);
}

void restart_heap_full(void) /* __attribute__ ((noreturn)) */
{
    ASSERT(restart_buffers_valid);
    longjmp(eval_restart, RR_HEAP_FULL);
}
