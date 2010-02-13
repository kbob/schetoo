#include "low_ex.h"

#include <assert.h>

#include "except.h"

static lowex_handler_t handler;

extern void send_sig_exception(obj_t ex)
{
    assert(handler);
    handler(LT_SIGNALLED, ex);
    assert(false);
}

extern void send_exception(obj_t ex)
{
    assert(handler);
    handler(LT_THROWN, ex);
    assert(false);
}

extern void send_heap_full(void)
{
    assert(handler);
    handler(LT_HEAP_FULL, NULL);
    assert(false);
}

extern void register_lowex_handler(lowex_handler_t new_handler)
{
    assert(handler == NULL);
    handler = new_handler;
}

extern void deregister_lowex_handler(lowex_handler_t old_handler)
{
    assert(handler == old_handler);
    handler = NULL;
}
