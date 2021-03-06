#ifndef EVAL_INCLUDED
#define EVAL_INCLUDED

#include "obj.h"

extern obj_t core_eval_cont(volatile obj_t cont,
			    volatile obj_t values,
			    obj_t handler);
extern obj_t core_eval(obj_t expr, obj_t env);

#endif /* !EVAL_INCLUDED */
