#ifndef OBJ_RTD_INCLUDED
#define OBJ_RTD_INCLUDED

/* rtd - record type descriptor */

#include "except.h"
#include "mem.h"

typedef enum rtd_flags {
    RF_SEALED        = 1 << 0,
    RF_OPAQUE        = 1 << 1,
    RF_NONGENERATIVE = 1 << 2,
    RF_SHIFT         = 3,
} rtd_flags_t;

typedef struct rtd_obj {
    mem_ops_t rtd_inst_ops;
    uword_t   rtd_flags;
    obj_t     rtd_name;
    obj_t     rtd_parent;
    obj_t     rtd_uid;
    obj_t     rtd_protocol;
    obj_t     rtd_fields;		// vector of ([im]mutable name)
} rtd_obj_t;

OBJ_TYPE_PREDICATE(rtd)			// bool is_rtd(obj_t);

extern obj_t make_rtd                    (rtd_flags_t flags,
					  obj_t       name,
					  obj_t       parent,
					  obj_t       uid,
					  obj_t       protocol,
					  obj_t       fields);

static inline bool   rtd_is_sealed       (obj_t       rtd);
static inline bool   rtd_is_opaque       (obj_t       rtd);
static inline bool   rtd_is_generative   (obj_t       rtd);
extern        bool   rtd_field_is_mutable(obj_t       rtd, size_t index);
extern        obj_t  rtd_field_name      (obj_t       rtd, size_t index);

static inline obj_t  rtd_name            (obj_t       rtd);
static inline obj_t  rtd_parent          (obj_t       rtd);
static inline obj_t  rtd_protocol        (obj_t       rtd);
static inline size_t rtd_field_count     (obj_t       rtd);
static inline obj_t  rtd_fields          (obj_t       rtd);

static inline bool rtd_is_sealed(obj_t rtd)
{
    CHECK(is_rtd(rtd), "must be rtd", rtd);
    return ((rtd_obj_t *)rtd)->rtd_flags & RF_SEALED ? true : false;
}

static inline bool rtd_is_opaque(obj_t rtd)
{
    CHECK(is_rtd(rtd), "must be rtd", rtd);
    return ((rtd_obj_t *)rtd)->rtd_flags & RF_OPAQUE ? true : false;
}

static inline bool rtd_is_generative(obj_t rtd)
{
    CHECK(is_rtd(rtd), "must be rtd", rtd);
    return ((rtd_obj_t *)rtd)->rtd_flags & RF_NONGENERATIVE ? false : true;
}

static inline obj_t rtd_name(obj_t rtd)
{
    CHECK(is_rtd(rtd), "must be rtd", rtd);
    return ((rtd_obj_t *)rtd)->rtd_name;
}

static inline obj_t rtd_parent(obj_t rtd)
{
    CHECK(is_rtd(rtd), "must be rtd", rtd);
    return ((rtd_obj_t *)rtd)->rtd_parent;
}

static inline obj_t rtd_protocol(obj_t rtd)
{
    CHECK(is_rtd(rtd), "must be rtd", rtd);
    return ((rtd_obj_t *)rtd)->rtd_protocol;
}

static inline size_t rtd_field_count(obj_t rtd)
{
    CHECK(is_rtd(rtd), "must be rtd", rtd);
    return ((rtd_obj_t *)rtd)->rtd_flags >> RF_SHIFT;
}

static inline obj_t rtd_fields(obj_t rtd)
{
    CHECK(is_rtd(rtd), "must be rtd", rtd);
    return ((rtd_obj_t *)rtd)->rtd_fields;
}

#endif /* !OBJ_RTD_INCLUDED */
