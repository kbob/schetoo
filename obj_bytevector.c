#include "obj_bytevector.h"

#include <string.h>

#include "except.h"
#include "mem.h"
#include "mem_scalar.h"
#include "obj_fixnum.h"
#include "obj_pair.h"

typedef struct bytevector_obj {
    heap_object_t v_header;
    size_t        v_size;
} bytevector_obj_t;

static inline size_t len_to_bytes(size_t len)
{
    return sizeof (bytevector_obj_t) + len * sizeof (byte_t);
}

static inline byte_t *elem_addr(bytevector_obj_t *vec, size_t index)
{
    return (void *)&vec[1] + index * sizeof (byte_t);
}

static size_t bytevector_size_op(const heap_object_t *oh)
{
    const bytevector_obj_t *vec = (const bytevector_obj_t *)oh;
    return len_to_bytes(vec->v_size);
}

static mem_ops_t bytevector_ops;

obj_t make_bytevector_uninitialized(size_t size)
{
    if (!bytevector_ops.mo_super)
	mem_scalar_create_ops(&bytevector_ops,
			      L"bytevector",
			      bytevector_size_op);
    heap_object_t *obj = mem_alloc_obj(&bytevector_ops, len_to_bytes(size));
    bytevector_obj_t *vec = (bytevector_obj_t *)obj;
    vec->v_size = size;
    return (obj_t)obj;
}

obj_t make_bytevector_fill(size_t size, byte_t fill)
{
    size_t i;

    obj_t obj = make_bytevector_uninitialized(size);
    bytevector_obj_t *vec = (bytevector_obj_t *)obj;
    for (i = 0; i < size; i++)
	*elem_addr(vec, i) = fill;
    return obj;
}

obj_t make_bytevector_from_list(obj_t list)
{
    obj_t p = list;
    size_t i, size = 0;
    while (!is_null(p)) {
	size++;
	p = pair_cdr(p);
    }
    obj_t bvec = make_bytevector_uninitialized(size);
    for (i = 0, p = list; i < size; i++) {
	bytevector_set(bvec, i, fixnum_value(pair_car(p)));
	p = pair_cdr(p);
    }
    return bvec;
}

bool is_bytevector(obj_t obj)
{
    check_obj(obj);
    return is_normal(obj) && obj_mem_ops(obj) == &bytevector_ops;
}

size_t bytevector_len(obj_t obj)
{
    check_obj(obj);
    bytevector_obj_t *vec = (bytevector_obj_t *)obj;
    return vec->v_size;
}

byte_t bytevector_get(obj_t obj, size_t index)
{
    check_obj(obj);
    bytevector_obj_t *vec = (bytevector_obj_t *)obj;
    CHECK(index < vec->v_size, obj, "bytevector_get: index out of range");
    return *elem_addr(vec, index);
}

void bytevector_set(obj_t obj, size_t index, byte_t elem)
{
    check_obj(obj);
    bytevector_obj_t *vec = (bytevector_obj_t *)obj;
    CHECK(index < vec->v_size, obj, "bytevector_set: index out of range");
    *elem_addr(vec, index) = elem;
}
