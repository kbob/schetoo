#ifndef EVAL_INCLUDED
#define EVAL_INCLUDED

#include "obj.h"

extern obj_t core_eval_cont(obj_t cont, obj_t values, obj_t handler);
extern obj_t core_eval(obj_t expr, obj_t env);

#endif /* !EVAL_INCLUDED */
