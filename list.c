#include "list.h"

#include "obj_null.h"
#include "obj_pair.h"

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
