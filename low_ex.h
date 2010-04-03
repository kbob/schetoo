#ifndef LOW_EX_INCLUDED
#define LOW_EX_INCLUDED

/* Low-level exceptions. */

#include "noreturn.h"
#include "obj.h"

typedef enum lowex_type {
    LT_NO_EXCEPTION = 0,
    LT_THROWN,
    LT_SIGNALLED,
    LT_HEAP_FULL,
} lowex_type_t;

typedef void (*lowex_handler_t)     (lowex_type_t type, obj_t ex) NORETURN;

/* send_sig_exception() - send an exception from a signal handler. */

NORETURN extern void send_exception          (obj_t ex);
NORETURN extern void send_sig_exception      (obj_t ex);
NORETURN extern void send_heap_full          (void);

         extern void register_lowex_handler  (lowex_handler_t);
         extern void deregister_lowex_handler(lowex_handler_t);

#endif /* !LOW_EX_INCLUDED */
