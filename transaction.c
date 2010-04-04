#include "transaction.h"

#ifndef NDEBUG

    #include <assert.h>
    #include <stdio.h>

    #include "heap.h"

    static __thread bool        retry_okay = true;
    static __thread const char *sf_func;
    static __thread int         sf_line;

    void commit(void)
    {
	commit_allocations();
	retry_okay = true;
    }

    void mutate(obj_t obj, const char *func, int line)
    {
	if (is_committed(obj)) {
	    SIDE_EFFECT();
	    sf_func = func;
	    sf_line = line;
	}
    }

    void side_effect(const char *func, int line)
    {
	sf_func = func;
	sf_line = line;
	retry_okay = false;
    }

    void could_retry(void)
    {
	if (!retry_okay)
	    fprintf(stderr, "last side effect at %s:%d\n", sf_func, sf_line);
	assert(retry_okay);
    }

#endif

