#ifndef EXCEPT_INCLUDED
#define EXCEPT_INCLUDED

#include "conditions.h"

// CHECK(expr, message, irritants...)
// message may be either a wide or 8-bit string.
#define CHECK(expr, ...)						\
    ((expr) ? (void)0							\
            : THROW(&assertion, __VA_ARGS__))

// THROW(condition, message, irritants...)
// Note that message may be either a wide or 8-bit string.
// An 8-bit string will be widened by the macro.
#define THROW(condition, ...)					\
    (throw((condition), L""  __VA_ARGS__, END_OF_ARGS))

__attribute__ ((noreturn))
extern void throw(obj_t *condition, const wchar_t *msg, ...);
    
extern void set_program_name(const char *);
extern const char *program_short_name(void);

#endif /* !EXCEPT_INCLUDED */
