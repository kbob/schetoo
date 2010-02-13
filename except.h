#ifndef EXCEPT_INCLUDED
#define EXCEPT_INCLUDED

#include "conditions.h"

// CHECK(expr, who, msg, irritants...)
// message may be either a wide or 8-bit string.
#define CHECK(expr, who, ...)						\
    ((expr) ? (void)0							\
            : THROW(&assertion, (who), __VA_ARGS__))

// THROW(condition, who, message, irritants...)
// Note that message may be either a wide or 8-bit string.
// An 8-bit string will be widened by the macro.
#define THROW(condition, who, ...)					\
    (throw((condition), (who), L""  __VA_ARGS__, END_OF_ARGS))

extern void throw(obj_t *condition, obj_t obj, const wchar_t *msg, ...)
					       __attribute__ ((noreturn));
    
extern void set_program_name(const char *);
extern const char *program_short_name(void);

#endif /* !EXCEPT_INCLUDED */
