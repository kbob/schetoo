#ifndef OBJ_BYTEVECTOR_INCLUDED
#define OBJ_BYTEVECTOR_INCLUDED

#include "mem.h"

typedef unsigned char byte_t;

extern obj_t   make_bytevector(size_t len, byte_t fill);
extern obj_t   make_bytevector_uninitialized(size_t len);
extern bool    is_bytevector(obj_t);
extern size_t  bytevector_len(obj_t);
extern byte_t  bytevector_get(obj_t, size_t index);
extern void    bytevector_set(obj_t, size_t index, byte_t elem);
extern byte_t *bytevector_addr(obj_t);

#endif /* !OBJ_BYTEVECTOR_INCLUDED */
