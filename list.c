#include "list.h"

#include "obj_null.h"
#include "obj_pair.h"

obj_t make_list_(obj_t first, ...)
{
    obj_t p = first, list = EMPTY_LIST;
    va_list ap;
    va_start(ap, first);
    while (p != END_OF_ARGS) {
	list = CONS(p, list);
	p = va_arg(ap, obj_t);
    }
    va_end(ap);
    return reverse_list(list);
}

size_t list_length(obj_t list)
{
    size_t n = 0;
    while (!is_null(list)) {
	n++;
	list = CDR(list);
    }
    return n;
}

obj_t reverse_list(obj_t list)
{
    obj_t rev = EMPTY_LIST;
    while (!is_null(list)) {
	rev = CONS(CAR(list), rev);
	list = CDR(list);
    }
    return rev;
}
