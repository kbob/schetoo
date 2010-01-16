#include "roots.h"

#include <wchar.h>

#include "except.h"

#define ROOTS_TRACE 0
#if ROOTS_TRACE
    #include <stdio.h>
#endif

static root_descriptor_t *static_roots;
static root_descriptor_t *thread_roots;

#if 0

void record_static_root(root_descriptor_t *desc)
{
    ASSERT(thread_roots == NULL);
    /* with lock */ {
	desc->rd_next = static_roots;
	static_roots = desc;
    }
}

void push_root(root_descriptor_t *desc)
{
#if ROOTS_TRACE
    printf("push_root(%s.%ls)\n", desc->rd_func, desc->rd_name);
#endif
    ASSERT(thread_roots < (root_descriptor_t *)0xc0000000 ||
	   desc < thread_roots);
    if (!thread_roots)
	thread_roots = static_roots;
    desc->rd_next = thread_roots;
    thread_roots = desc;
}

void pop_function_roots(const char *func)
{
#if ROOTS_TRACE
    printf("pop_function_roots(%s)\n", func);
#endif
    while (thread_roots && thread_roots->rd_func == func)
	pop_root(thread_roots->rd_name);
#if ROOTS_TRACE
    printf("\n");
#endif
#ifndef NDEBUG
    root_descriptor_t *p;
#if ROOTS_TRACE
    printf("stack =");
    for (p = thread_roots; p; p = p->rd_next)
	printf(" %s.%ls", p->rd_func, p->rd_name);
    printf("\n");
#endif
    for (p = thread_roots; p; p = p->rd_next)
	ASSERT(p->rd_func != func);
#endif
}

void pop_root(const wchar_t *name)
{
#if ROOTS_TRACE
    printf("pop_root(%ls)\n", name);
#endif
    ASSERT(!wcscmp(thread_roots->rd_name, name));
    thread_roots = thread_roots->rd_next;
}

void init_roots(void)
{
    root_descriptor_t *p = static_roots;
    while (p) {
	if (p->rd_init) {
	    *p->rd_root = p->rd_init();
	}
	p = p->rd_next;
    }
}

#endif

root_descriptor_t *get_thread_roots(void)
{
    if (thread_roots)
	return thread_roots;
    else
	return static_roots;
}
