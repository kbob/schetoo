#include "obj_vector.h"

#include "except.h"
#include "mem.h"
#include "obj_fixnum.h"
#include "obj_pair.h"

typedef struct vector_obj {
    heap_object_t v_header;
    size_t        v_size;
} vector_obj_t;

static inline size_t len_to_bytes(size_t len)
{
    return sizeof (vector_obj_t) + len * sizeof (obj_t);
}

static inline obj_t *elem_addr(vector_obj_t *vec, size_t index)
{
    return (obj_t *)&vec[1] + index;
}

static size_t vector_size_op(const heap_object_t *obj)
{
    const vector_obj_t *vec = (const vector_obj_t *)obj;
    return len_to_bytes(vec->v_size);
}

static size_t vector_ptr_count_op(const heap_object_t *obj)
{
    const vector_obj_t *vec = (const vector_obj_t *)obj;
    return vec->v_size;
}

static void vector_move_op(const heap_object_t *src, heap_object_t *dst)
{
    vector_obj_t *vsrc = (vector_obj_t *)src;
    vector_obj_t *vdst = (vector_obj_t *)dst;
    *vdst = *vsrc;
    size_t i, size = vsrc->v_size;
    for (i = 0; i < size; i++)
	*elem_addr(vdst, i) = *elem_addr(vsrc, i);
}

static obj_t vector_get_ptr_op(const heap_object_t *obj, size_t index)
{
    vector_obj_t *vec = (vector_obj_t *)obj;
    ASSERT(index < vec->v_size);
    return *elem_addr(vec, index);
}

static void vector_set_ptr_op(heap_object_t *obj, size_t index, obj_t ptr)
{
    vector_obj_t *vec = (vector_obj_t *)obj;
    ASSERT(index < vec->v_size);
    *elem_addr(vec, index) = ptr;
}

static mem_ops_t vector_ops = {
    MEM_OPS_PRIMITIVE,
    L"vector",
    NULL,
    vector_size_op,
    vector_ptr_count_op,
    vector_move_op,
    vector_get_ptr_op,
    vector_set_ptr_op,
    { }
};

obj_t make_vector_uninitialized(size_t size)
{
    heap_object_t *obj = mem_alloc_obj(&vector_ops, len_to_bytes(size));
    vector_obj_t *vec = (vector_obj_t *)obj;
    vec->v_size = size;
    return (obj_t)obj;
}

obj_t make_vector_fill(size_t size, obj_t fill)
{
    size_t i;

    CHECK_OBJ(fill);
    obj_t obj = make_vector_uninitialized(size);
    vector_obj_t *vec = (vector_obj_t *)obj;
    for (i = 0; i < size; i++)
	*elem_addr(vec, i) = fill;
    return obj;
}

obj_t make_vector_from_list(obj_t list)
{
    obj_t p = list;
    size_t i, size = 0;
    while (!is_null(p)) {
	size++;
	p = pair_cdr(p);
    }
    obj_t vec = make_vector_uninitialized(size);
    for (i = 0, p = list; i < size; i++) {
	vector_set(vec, i, pair_car(p));
	p = pair_cdr(p);
    }
    return vec;
}

bool is_vector(obj_t obj)
{
    CHECK_OBJ(obj);
    return is_heap(obj) && obj_mem_ops(obj) == &vector_ops;
}

size_t vector_len(obj_t obj)
{
    CHECK_OBJ(obj);
    vector_obj_t *vec = (vector_obj_t *)obj;
    return vec->v_size;
}

obj_t vector_ref(obj_t obj, size_t index)
{
    CHECK_OBJ(obj);
    CHECK(is_vector(obj), obj, "vector-ref: must be vector");
    vector_obj_t *vec = (vector_obj_t *)obj;
    CHECK(index < vec->v_size,
	  obj, "vector-ref: index out of range", make_fixnum(index));
    return *elem_addr(vec, index);
}

void vector_set(obj_t obj, size_t index, obj_t elem)
{
    CHECK_OBJ(obj);
    CHECK(is_vector(obj), obj, "vector-set! must be vector");
    vector_obj_t *vec = (vector_obj_t *)obj;
    CHECK(index < vec->v_size,
	  obj, "vector-set! index out of range", make_fixnum(index));
    *elem_addr(vec, index) = elem;
}
