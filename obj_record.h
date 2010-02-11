#ifndef OBJ_RECORD_INCLUDED
#define OBJ_RECORD_INCLUDED

#include "mem.h"
#include "obj_rtd.h"

extern mem_ops_t record_ops;

typedef struct record_obj {
    obj_t rec_rtd;
} record_obj_t;

OBJ_TYPE_PREDICATE(record)		// bool is_record(obj_t);

extern        obj_t make_record        (obj_t rtd, ...);

static inline bool  is_nonopaque_record(obj_t obj);
static inline bool  is_instance        (obj_t rec, obj_t rtd);

static inline obj_t record_rtd         (obj_t rec);
extern        obj_t record_get_field   (obj_t rec, size_t index);
extern        void  record_set_field   (obj_t rec, size_t index, obj_t value);

static inline bool is_nonopaque_record(obj_t obj)
{
    return is_record(obj) &&
	   !rtd_is_opaque(((record_obj_t *)obj)->rec_rtd);
}

static inline bool is_instance(obj_t rec, obj_t rtd)
{
    CHECK(is_record(rec), NULL, "must be record", rec);
    CHECK(is_rtd(rtd),    NULL, "must be rtd",    rtd);
    return ((record_obj_t *)rec)->rec_rtd == rtd;
}

static inline obj_t record_rtd(obj_t rec)
{
    return ((record_obj_t *)rec)->rec_rtd;
}

#endif /* !OBJ_RECORD_INCLUDED */
