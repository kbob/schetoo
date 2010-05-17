/* Three different ways of organizing the registers */

#include <stdio.h>
#include <sys/time.h>

//#define volatile

#define REPS 1000000000

/* First way: two args, env in cont. */

typedef struct cv {
    void *cont;
    void *values;
} cv_t;

typedef struct cve {
    void *cont;
    void *values;
    void *env;
} cve_t;

static inline cv_t cv(void *cont, void *values)
{
    cv_t x = { cont, values };
    return x;
}

static inline cve_t cve(void *cont, void *values, void *env)
{
    cve_t x = { cont, values, env };
    return x;
}

#define STACK_SIZE 4000
int stack[STACK_SIZE];
int *sp = stack, *s_end = &stack[STACK_SIZE];
int counter;

typedef struct cont3 {
    void *op;
    void *arg;
    void *cont;
} cont3_t;

typedef struct cont4 {
    void *op;
    void *arg;
    void *env;
    void *cont;
} cont4_t;

cont3_t *make_cont3(void *op, void *arg, void *cont)
{
    if (sp + 3 >= s_end)
	sp = stack;
    cont3_t *p = (cont3_t *)(sp++);
    p->op = op;
    p->arg = arg;
    p->cont = cont;
    return p;
}

cont4_t *make_cont4(void *op, void *arg, void *env, void *cont)
{
    if (sp + 4 >= s_end)
	sp = stack;
    cont4_t *p = (cont4_t *)(sp++);
    p->op = op;
    p->arg = arg;
    p->env = env;
    p->cont = cont;
    return p;
}

/*****************************************************************************/

typedef cv_t (*cont_proc_v1_t)(cont4_t *cont, void *values);

cv_t an_op_v1(cont4_t *cont, char *values)
{
    ++counter;
    return cv(make_cont4(an_op_v1, NULL, cont->env, cont->cont), values + 1);
}

int dispatch_v1(void)
{
    volatile char *values = 0;
    volatile cont4_t *cont = make_cont4(an_op_v1, NULL, NULL, NULL);
    int i;
    for (i = 0; i < REPS; i++) {
	cv_t reg = ((cont_proc_v1_t)cont->op)((cont4_t *)cont, (char *)values);
	cont = reg.cont;
	values = reg.values;
    }
    return (int)values;
}

/*****************************************************************************/

typedef cve_t (*cont_proc_v2_t)(cont3_t *cont, void *values, void *env);

cve_t an_op_v2(cont3_t *cont, void *values, void *env)
{
    ++counter;
    return cve(make_cont3(an_op_v2, NULL, cont->cont), values + 1, env);
}

int dispatch_v2(void)
{
    volatile char *values = 0;
    volatile cont3_t *cont = make_cont3(an_op_v2, NULL, NULL);
    volatile void *env = 0;
    int i;
    for (i = 0; i < REPS; i++) {
	cve_t reg = ((cont_proc_v2_t)cont->op)((cont3_t *)cont,
					       (char    *)values,
					       (void    *)env);
	cont = reg.cont;
	values = reg.values;
	env = reg.env;
    }
    return (int)values;
}

/*****************************************************************************/

void *env_v3;

typedef cv_t (*cont_proc_v3_t)(cont3_t *cont, void *values);

cv_t an_op_v3(cont3_t *cont, void *values, void *env)
{
    if (++counter % 10 == 0)
	env_v3 = values;
    return cv(make_cont3(an_op_v3, NULL, cont->cont), values + 1);
}

int dispatch_v3(void)
{
    volatile char *values = 0;
    volatile cont3_t *cont = make_cont3(an_op_v3, NULL, NULL);
    int i;
    for (i = 0; i < REPS; i++) {
	cv_t reg = ((cont_proc_v3_t)cont->op)((cont3_t *)cont, (char *)values);
	cont = reg.cont;
	values = reg.values;
    }
    return (int)values;
}

/*****************************************************************************/

double ftime(struct timeval *tp)
{
    return (double)tp->tv_sec + (double)tp->tv_usec / 1000000.0;
}

void benchmark(const char *label, int (*f)(void))
{
    struct timeval before, after;
    counter = 0;
    gettimeofday(&before, NULL);
    int x = (*f)();
    gettimeofday(&after, NULL);
    printf("%s result %d time %g\n", label, x, ftime(&after) - ftime(&before));
}

int main()
{
    benchmark("version 1", dispatch_v1);
    benchmark("version 2", dispatch_v2);
    benchmark("version 3", dispatch_v3);
    return 0;
}
