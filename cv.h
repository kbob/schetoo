#ifndef CV_INCLUDED
#define CV_INCLUDED

#include "obj.h"

/*
 * A continuation bundles some values together with a continuation
 * proc.  A continuation proc returns two values: a continuation and a
 * list of evaluated expressions.  Since C doesn't have multivalued
 * functions, continuation procs are defined as returning a cv_t
 * (continuation + values type).
 *
 * By convention, continuation procs' names start with "c_".
 */

typedef struct cv {
    obj_t cv_cont;
    obj_t cv_values;
} cv_t;

typedef cv_t (*cont_proc_t)(obj_t cont, obj_t values);

/* cv() - cv_t constructor. */

static inline cv_t cv(obj_t cont, obj_t values)
{
    cv_t tmp = { cont, values };
    return tmp;
}

#endif /* !CV_INCLUDED */
