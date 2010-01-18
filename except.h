#ifndef EXCEPT_INCLUDED
#define EXCEPT_INCLUDED

#include "obj.h"

#ifdef NDEBUG
    #define ASSERT(expr) ((void)0)
#else
    #define ASSERT(expr)                                                       \
        ((expr) ? (void)0                                                      \
                : assertion_failed(__FILE__, __LINE__, __func__, # expr))
#endif

// CHECK(expr, who, msg, irritants...)
#define CHECK(expr, who, ...) \
    ((expr) ? (void)0 : raise(&violation, (who), __VA_ARGS__))

typedef struct condition_type {
    const char *ct_name;
} condition_type_t;

extern condition_type_t condition, warning, serious, message, irritants, who;
extern condition_type_t error, violation, assertion, non_continuable;
extern condition_type_t implementation_restriction, lexical, syntax, undefined;

#ifndef NDEBUG
extern void assertion_failed(const char *file,
			     int	 line,
			     const char *fn,
			     const char *expr) __attribute__ ((noreturn));
#endif

extern void raise_error     (obj_t       who,
			     const char *message,
			     obj_t       irritants,
			     ...)              __attribute__ ((noreturn));

extern void raise           (condition_type_t *,
		             obj_t       obj,
		             const char *msg,
			     ...)              __attribute__ ((noreturn));
    
extern void raise_continuable(condition_type_t *,
			      obj_t       obj,
			      const char *msg) __attribute__ ((noreturn));

extern void set_program_name(const char *);

#endif /* !EXCEPT_INCLUDED */
