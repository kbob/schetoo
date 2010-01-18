#include "mem_scalar.h"

#include <string.h>

#include "except.h"

static size_t scalar_ptr_count_op(const heap_object_t *obj)
{
    return 0;
}

static void scalar_move_op(const heap_object_t *src, heap_object_t *dst)
{
    memcpy(dst, src, heap_object_mem_ops((heap_object_t *)src)->mo_size(src));
}

static obj_t scalar_get_ptr_op(const heap_object_t *obj, size_t index)
{
    ASSERT(false);
    return 0;
}

static void scalar_set_ptr_op(heap_object_t *obj, size_t index, obj_t ptr)
{
    ASSERT(false);
}

static mem_ops_t scalar_ops = {
    MEM_OPS_PRIMITIVE,
    NULL,
    NULL,
    NULL,
    scalar_ptr_count_op,
    scalar_move_op,
    scalar_get_ptr_op,
    scalar_set_ptr_op,
    { }
};

void mem_scalar_create_ops(mem_ops_t *ops, wchar_t *name, mem_size_op size_op)
{
    *ops = scalar_ops;
    ops->mo_super = &scalar_ops;
    ops->mo_name = name;
    ops->mo_size = size_op;
}
