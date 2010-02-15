#include "obj_record.h"

#include <assert.h>
#include <stdarg.h>

#include "obj_fixnum.h"

static inline size_t len_to_bytes(size_t len)
{
    return sizeof (record_obj_t) + len * sizeof (obj_t);
}

static inline obj_t *elem_addr(record_obj_t *rec, size_t index)
{
    return (obj_t *)&rec[1] + index;
}

static size_t rec_size_op(const heap_object_t *hobj)
{
    record_obj_t *rec = (record_obj_t *)hobj;
    return len_to_bytes(rtd_field_count_unchecked(rec->rec_rtd));
}

static size_t rec_ptr_count_op(const heap_object_t *hobj)
{
    record_obj_t *rec = (record_obj_t *)hobj;
    return 1 + rtd_field_count_unchecked(rec->rec_rtd);
}

static void rec_move_op(const heap_object_t *src, heap_object_t *dst)
{
    record_obj_t *rsrc = (record_obj_t *)src;
    record_obj_t *rdst = (record_obj_t *)dst;
    *rdst = *rsrc;
    size_t i, size = rtd_field_count_unchecked(rsrc->rec_rtd);
    for (i = 0; i < size; i++)
	*elem_addr(rdst, i) = *elem_addr(rsrc, i);
}

static obj_t rec_get_ptr_op(const heap_object_t *hobj, size_t index)
{
    record_obj_t *rec = (record_obj_t *)hobj;
    if (index == 0)
	return rec->rec_rtd;
    else if (index <= rtd_field_count_unchecked(rec->rec_rtd))
	return *elem_addr(rec, index - 1);
    else
	assert(false && "index out of range");
}

static void rec_set_ptr_op(heap_object_t *hobj, size_t index, obj_t ptr)
{
    record_obj_t *rec = (record_obj_t *)hobj;
    if (index == 0)
	rec->rec_rtd = ptr;
    else if (index <= rtd_field_count_unchecked(rec->rec_rtd))
	*elem_addr(rec, index - 1) = ptr;
    else
	assert(false && "index out of range");
}

mem_ops_t record_ops = {
    MEM_OPS_PRIMITIVE,
    L"record",
    NULL,
    rec_size_op,
    rec_ptr_count_op,
    rec_move_op,
    rec_get_ptr_op,
    rec_set_ptr_op,
};

obj_t make_record_(obj_t rtd, ...)
{
    CHECK(is_rtd(rtd), "must be rtd", rtd);
    size_t i, size = rtd_field_count(rtd);
    heap_object_t *hobj = mem_alloc_obj((mem_ops_t *)rtd, len_to_bytes(size));
    record_obj_t *rec = (record_obj_t *)hobj;
    va_list ap;
    va_start(ap, rtd);
    for (i = 0; i < size; i++) {
	obj_t field = va_arg(ap, obj_t);
	CHECK_OBJ(field);
	assert(field != END_OF_ARGS);
	*elem_addr(rec, i) = field;
    }
    obj_t end_marker = va_arg(ap, obj_t);
    assert(end_marker == END_OF_ARGS);
    va_end(ap);
    return (obj_t)hobj;
}

obj_t record_get_field(obj_t obj, size_t index)
{
    CHECK(is_record(obj), "must be record", obj);
    record_obj_t *rec = (record_obj_t *)obj;
    CHECK(index < rtd_field_count(rec->rec_rtd), "index out of range",
	  obj, make_fixnum(index));
    return *elem_addr(rec, index);
}

void record_set_field(obj_t obj, size_t index, obj_t value)
{
    CHECK(is_record(obj), "must be record", obj);
    record_obj_t *rec = (record_obj_t *)obj;
    CHECK(index < rtd_field_count(rec->rec_rtd), "index out of range",
	  obj, make_fixnum(index));
    CHECK_OBJ(value);
    *elem_addr(rec, index) = value;
}
