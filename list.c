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

bool is_list(obj_t obj)
{
    while (is_pair(obj))
	obj = pair_cdr(obj);
    return is_null(obj);
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

size_t irregular_list_length(obj_t list)
{
    size_t n = 0;
    while (is_pair(list)) {
	n++;
	list = CDR(list);
    }
    return n + !is_null(list);
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

obj_t cons_if_changed(obj_t old_pair, obj_t new_car, obj_t new_cdr)
{
    assert(is_pair(old_pair));
    if (new_car == pair_car(old_pair) && new_cdr == pair_cdr(old_pair))
	return old_pair;
    return make_pair(new_car, new_cdr);
}
