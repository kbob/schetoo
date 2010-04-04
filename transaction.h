#ifndef TRANSACTION_INCLUDED
#define TRANSACTION_INCLUDED

#ifdef NDEBUG

    #define COMMIT()      ((void)0)
    #define MUTATE(obj)   ((void)0)
    #define SIDE_EFFECT() ((void)0)
    #define COULD_RETRY() ((void)0)

#else

    #include "obj.h"

    /*
     * When debugging, we can check that no allocations occur between
     * the time the heap is mutated and the next commit.
     *
     * If an allocation did occur, it could trigger a GC which would
     * cause the whole operation to be retried.
     */

    #define COMMIT()       (commit())
    #define MUTATE(obj)    (mutate(obj, __func__, __LINE__))
    #define SIDE_EFFECT()  (side_effect(__func__, __LINE__))
    #define COULD_RETRY()  (could_retry())

    extern void commit     (void);
    extern void mutate     (obj_t, const char *func, int line);
    extern void side_effect(       const char *func, int line);
    extern void could_retry(void);

#endif

#endif /* !TRANSACTION_INCLUDED */
