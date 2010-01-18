#include "roots.h"

#include "except.h"

static root_descriptor_t *static_roots;
static root_descriptor_t *thread_roots;

void record_static_root(root_descriptor_t *desc)
{
    ASSERT(thread_roots == NULL);
    /* with lock */ {
	desc->rd_next = static_roots;
	static_roots = desc;
    }
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

root_descriptor_t *get_thread_roots(void)
{
    if (thread_roots)
	return thread_roots;
    else
	return static_roots;
}
