#ifndef LIST_INCLUDED
#define LIST_INCLUDED

#include "obj.h"

/* list - common list utilities */

#define MAKE_LIST(...) (make_list_(__VA_ARGS__, END_OF_ARGS))

extern obj_t make_list_(obj_t first, ...);

extern size_t list_length(obj_t list);

extern obj_t reverse_list(obj_t list);

#endif /* !LIST_INCLUDED */
