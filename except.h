#ifndef EXCEPT_INCLUDED
#define EXCEPT_INCLUDED

#include "conditions.h"

#ifdef NDEBUG
    #define ASSERT(expr) ((void)0)
#else
    #define ASSERT(expr)						\
        ((expr) ? (void)0						\
                : assertion_failed(__FILE__, __LINE__, __func__, # expr))
#endif

// CHECK(expr, who, msg, irritants...)
#if 0
#define CHECK(expr, who, ...)						\
    ((expr) ? (void)0							\
            : raise(&assertion, (who), L"" __VA_ARGS__, END_OF_ARGS))
#endif

#define CHECK(expr, who, ...)						\
    ((expr) ? (void)0							\
            : RAISE(&assertion, (who), __VA_ARGS__))

#ifndef NDEBUG
extern void assertion_failed(const char *file,
			     int	 line,
			     const char *fn,
			     const char *expr) __attribute__ ((noreturn));
#endif

// RAISE(condition, who, message, irritants...)
#define RAISE(condition, who, ...)					\
    (raise((condition), (who), L""  __VA_ARGS__, END_OF_ARGS))

extern void raise_error     (obj_t          who,
			     const wchar_t *message,
			     obj_t          irritants,
			     ...)              __attribute__ ((noreturn));

extern void raise           (obj_t         *condition,
		             obj_t          obj,
		             const wchar_t *msg,
			     ...)              __attribute__ ((noreturn));
    
extern void raise_continuable(obj_t         *condition,
			      obj_t          obj,
			      const wchar_t *msg) __attribute__ ((noreturn));

extern void set_program_name(const char *);
extern const char *program_short_name(void);

#endif /* !EXCEPT_INCLUDED */
