#include "proc.h"

#include "types.h"

DEFINE_PROC(L"+", 0-)(obj_t arglist)
{
    word_t sum = 0;
    while (!is_null(arglist)) {
	sum += fixnum_value(CAR(arglist));
	arglist = CDR(arglist);
    }
    return make_fixnum(sum);
}
