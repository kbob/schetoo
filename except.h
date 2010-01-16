#ifndef EXCEPT_INCLUDED
#define EXCEPT_INCLUDED

#include "mem.h"

#ifdef NDEBUG
    #define ASSERT(expr) ((void)0)
#else
    #define ASSERT(expr)                                                       \
        ((expr) ? (void)0                                                      \
                : assertion_failed(__FILE__, __LINE__, __FUNCTION__, # expr))
#endif

#define CHECK(expr, who, msg, ...) \
    ((expr) ? (void)0 : raise(&violation, (who), (msg) __VA_ARGS__))

typedef struct condition_type {
    const char *ct_name;
} condition_type_t;

extern condition_type_t condition, warning, serious, message, irritants, who;
extern condition_type_t error, violation, assertion, non_continuable;
extern condition_type_t implementation_restriction, lexical, syntax, undefined;

extern void assertion_failed(const char *file,
			     int	 line,
			     const char *fn,
			     const char *expr) __THROW __attribute__ ((__noreturn__));

extern void raise_error(obj_t who, const char *message, obj_t irritants, ...);

extern void raise(condition_type_t *, obj_t obj, const char *msg);
    
extern void raise_continuable(condition_type_t *, obj_t obj, const char *msg);

extern void set_program_name(const char *);

#endif /* !EXCEPT_INCLUDED */
