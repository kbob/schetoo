#include "transaction.h"

#ifndef NDEBUG

    #include <assert.h>

    #include "heap.h"

    static bool retry_okay;

    void commit(void)
    {
	commit_allocations();
    }

    void mutate(obj_t obj)
    {
	if (is_committed(obj))
	    SIDE_EFFECT();
    }

    void side_effect(void)
    {
	retry_okay = false;
    }

    void could_retry(void)
    {
	assert(retry_okay);
    }

#endif

