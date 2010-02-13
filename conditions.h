#ifndef CONDITIONS_INCLUDED
#define CONDITIONS_INCLUDED

#include <stdarg.h>

#include "obj.h"

extern obj_t condition, warning, serious, message, irritants, who;
extern obj_t error, violation, assertion, non_continuable;
extern obj_t implementation_restriction, lexical, syntax, undefined;

extern obj_t make_compound_condition_(obj_t parts, ...);

#define MAKE_COMPOUND_CONDITION(...) \
    (make_compound_condition_(__VA_ARGS__, END_OF_ARGS))

#endif /* !CONDITIONS_INCLUDED */
