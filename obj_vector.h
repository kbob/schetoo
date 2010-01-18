#ifndef OBJ_VECTOR_INCLUDED
#define OBJ_VECTOR_INCLUDED

#include "obj.h"

extern obj_t  make_vector_uninitialized(size_t len);
extern obj_t  make_vector_fill         (size_t len, obj_t fill);
extern obj_t  make_vector_from_list    (obj_t list);
extern bool   is_vector                (obj_t);
extern size_t vector_len               (obj_t);
extern obj_t  vector_ref               (obj_t, size_t index);
extern void   vector_set               (obj_t vec, size_t index, obj_t elem);

#endif /* !OBJ_VECTOR_INCLUDED */
