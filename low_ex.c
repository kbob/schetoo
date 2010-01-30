#include "low_ex.h"

#include "except.h"

static lowex_handler_t handler;

extern void send_sig_exception(obj_t ex)
{
    ASSERT(handler);
    handler(LT_SIGNALLED, ex);
    ASSERT(false);
}

extern void send_exception(obj_t ex)
{
    ASSERT(handler);
    handler(LT_THROWN, ex);
    ASSERT(false);
}

extern void send_heap_full(void)
{
    ASSERT(handler);
    handler(LT_HEAP_FULL, NULL);
    ASSERT(false);
}

extern void register_lowex_handler(lowex_handler_t new_handler)
{
    ASSERT(handler == NULL);
    handler = new_handler;
}

extern void deregister_lowex_handler(lowex_handler_t old_handler)
{
    ASSERT(handler == old_handler);
    handler = NULL;
}
