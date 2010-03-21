#ifndef EVAL_INCLUDED
#define EVAL_INCLUDED

#include "obj.h"

extern obj_t core_eval_cont(obj_t cont);
extern obj_t core_eval(obj_t expr, obj_t env);

#endif /* !EVAL_INCLUDED */
