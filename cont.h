#ifndef CONT_INCLUDED
#define CONT_INCLUDED

#include "cv.h"
#include "obj.h"

// PUSH_APP(cont, proc, args...)
#define PUSH_APP(cont, ...) (push_application(cont, __VA_ARGS__, END_OF_ARGS))

extern obj_t push_application(obj_t cont, cont_proc_t proc, ...);

// extern obj_t push_open_application(cont, proc);

#endif /* !CONT_INCLUDED */
