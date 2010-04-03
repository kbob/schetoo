#ifndef OBJ_RECORD_INCLUDED
#define OBJ_RECORD_INCLUDED

#include "mem.h"
#include "obj_boolean.h"
#include "obj_rtd.h"

// MAKE_RECORD(rtd, field_values...)
#define MAKE_RECORD(...) (make_record_(__VA_ARGS__, END_OF_ARGS))

extern mem_ops_t record_ops;

typedef struct record_obj {
    obj_t rec_rtd;
} record_obj_t;

extern        obj_t  make_record_       (obj_t rtd, ...);
extern        obj_t  make_record_va     (obj_t rtd, va_list fields);

static inline bool   is_record          (obj_t obj);
static inline bool   is_nonopaque_record(obj_t obj);
static inline bool   is_instance        (obj_t rec, obj_t rtd);

static inline obj_t  record_rtd         (obj_t rec);
static inline size_t record_len         (obj_t rec);
extern        obj_t  record_field_name  (obj_t rec, size_t index);
extern        obj_t  record_get_field   (obj_t rec, size_t index);
extern        void   record_set_field   (obj_t rec, size_t index, obj_t value);

static inline bool is_record(obj_t obj)
{
    return (is_heap(obj) &&
	    ((heap_object_t *)obj)->ho_ops->mo_start_marker !=
	    MEM_OPS_PRIMITIVE);
}

static inline bool is_nonopaque_record(obj_t obj)
{
    return is_record(obj) &&
	   !rtd_is_opaque(((record_obj_t *)obj)->rec_rtd);
}

static inline bool is_instance(obj_t rec, obj_t rtd)
{
    CHECK(is_record(rec), "must be record", rec);
    CHECK(is_rtd(rtd), "must be rtd",    rtd);
    //return ((record_obj_t *)rec)->rec_rtd == rtd;
    obj_t p = record_rtd(rec);
    while (p != FALSE_OBJ) {
	if (p == rtd)
	    return true;
	p = rtd_parent(p);
    }
    return false;
}

static inline obj_t record_rtd(obj_t rec)
{
    CHECK(is_record(rec), "must be record", rec);
    return ((record_obj_t *)rec)->rec_rtd;
}

static inline size_t record_len(obj_t rec)
{
    return rtd_field_count(record_rtd(rec));
}

#endif /* !OBJ_RECORD_INCLUDED */
