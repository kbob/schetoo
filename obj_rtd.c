#include "obj_rtd.h"

#include "obj_boolean.h"
#include "obj_fixnum.h"
#include "obj_pair.h"
#include "obj_record.h"
#include "obj_symbol.h"
#include "obj_vector.h"

static size_t rtd_size_op(const heap_object_t *hobj)
{
    return sizeof (rtd_obj_t);
}

static size_t rtd_ptr_count_op(const heap_object_t *hobj)
{
    return 5;
}

static void rtd_move_op(const heap_object_t *src, heap_object_t *dst)
{
    *(rtd_obj_t *)dst = *(const rtd_obj_t *)src;
}

static obj_t rtd_get_ptr_op(const heap_object_t *hobj, size_t index)
{
    rtd_obj_t *rtd = (rtd_obj_t *)hobj;
    switch (index) {
    case 0:
	return rtd->rtd_name;
    case 1:
	return rtd->rtd_parent;
    case 2:
	return rtd->rtd_uid;
    case 3:
	return rtd->rtd_protocol;
    case 4:
	return rtd->rtd_fields;
    }
    ASSERT(false && "index out of range");
}

static void rtd_set_ptr_op(heap_object_t *hobj, size_t index, obj_t ptr)
{
    rtd_obj_t *rtd = (rtd_obj_t *)hobj;
    switch (index) {

    case 0:
	rtd->rtd_name = ptr;
	break;

    case 1:
	rtd->rtd_parent = ptr;
	break;

    case 2:
	rtd->rtd_uid = ptr;
	break;

    case 3:
	rtd->rtd_protocol = ptr;
	break;

    case 4:
	rtd->rtd_fields = ptr;
	break;

    default:
	ASSERT(false && "index out of range");
    }
}

mem_ops_t rtd_ops = {
    MEM_OPS_PRIMITIVE,
    L"rtd",
    NULL,
    rtd_size_op,
    rtd_ptr_count_op,
    rtd_move_op,
    rtd_get_ptr_op,
    rtd_set_ptr_op,
    { }
};

obj_t make_rtd(rtd_flags_t flags,
	       obj_t       name,
	       obj_t       parent,
	       obj_t       uid,
	       obj_t       protocol,
	       obj_t       fields)
{
    CHECK_OBJ(name);
    CHECK_OBJ(parent);
    CHECK_OBJ(uid);
    CHECK_OBJ(protocol);
    CHECK_OBJ(fields);
    CHECK(is_symbol(name), NULL, "must be symbol", name);
    CHECK(parent == FALSE_OBJ || is_rtd(parent),
	  NULL, "must be rtd or #f", parent);
    CHECK(parent == FALSE_OBJ || !rtd_is_sealed(parent),
	  NULL, "parent is sealed", parent);
    if (protocol != FALSE_OBJ)
	RAISE(&implementation_restriction, NULL,
	      "record protocols not implemented", protocol);
    if ((flags & RF_NONGENERATIVE) || uid != FALSE_OBJ)
	RAISE(&implementation_restriction, NULL,
	      "nongenerative records not implemented", uid);
    CHECK(is_vector(fields), NULL, "must be vector", fields);
    // XXX walk through the fields and ensure they have the right format.
    // ... or wait until the first instantiation...
    heap_object_t *hobj = mem_alloc_obj(&rtd_ops, sizeof (rtd_obj_t));
    size_t field_count = parent == FALSE_OBJ ? 0 : rtd_field_count(parent);
    field_count += vector_len(fields);
    rtd_obj_t *rtd = (rtd_obj_t *) hobj;
    rtd->rtd_inst_ops = record_ops;
    hobj->ho_ops      = &rtd_ops;	// overwrite first word of rtd_inst_ops
    rtd->rtd_flags    = flags | field_count << RF_SHIFT;
    rtd->rtd_name     = name;
    rtd->rtd_parent   = parent;
    rtd->rtd_uid      = uid;
    rtd->rtd_protocol = protocol;
    rtd->rtd_fields   = fields;
    return (obj_t)hobj;
}

static obj_t find_field(obj_t rtd, size_t index)
{
    CHECK(is_rtd(rtd), NULL, "must be rtd", rtd);
    size_t n = rtd_field_count(rtd);
    size_t m = vector_len(((rtd_obj_t *)rtd)->rtd_fields);
    CHECK(index < n, NULL, "index out of range", make_fixnum(index));
    while (index < n - m) {
	n -= m;
	rtd = rtd_parent(rtd);
	ASSERT(is_rtd(rtd));
	m = vector_len(((rtd_obj_t *)rtd)->rtd_fields);
    }
    return vector_ref(((rtd_obj_t *)rtd)->rtd_fields, index - (n - m));
}

bool rtd_field_is_mutable(obj_t rtd, size_t index)
{
    return CAR(find_field(rtd, index)) == make_symbol_from_C_str(L"mutable");
}

obj_t rtd_field_name(obj_t rtd, size_t index)
{
    return CADR(find_field(rtd, index));
}
