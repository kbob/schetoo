#ifndef RECORD_INCLUDED
#define RECORD_INCLUDED

/* record - Macros to define record types from C. */

#include <stddef.h>
#include <stdio.h>

#include "obj.h"
#include "obj_rtd.h"
#include "roots.h"

#define DEFINE_RECORD_TYPE DEFINE_ANONYMOUS_RECORD_TYPE

#define DEFINE_EXTERN_RECORD_TYPE(C_name, scheme_name, parent, flags)	\
    DEFINE_GENERAL_RECORD_TYPE_(/* blank */,				\
				C_name,					\
				scheme_name,				\
				parent,					\
				flags,					\
				UNIQ_IDENT(record_fields_))

#define DEFINE_STATIC_RECORD_TYPE(C_name, scheme_name, parent, flags)	\
    DEFINE_GENERAL_RECORD_TYPE_(static,					\
				C_name,					\
				scheme_name,				\
				parent,					\
				flags,					\
				UNIQ_IDENT(record_fields_))

#define DEFINE_ANONYMOUS_RECORD_TYPE(scheme_name, parent, flags)	\
    DEFINE_GENERAL_RECORD_TYPE_(static,					\
				UNIQ_IDENT(anonymous_record_),		\
				scheme_name,				\
				parent,					\
				flags,					\
				UNIQ_IDENT(record_fields_))

#define DEFINE_GENERAL_RECORD_TYPE_(storage_class,			\
				    C_name,				\
				    scheme_name,			\
				    parent,				\
				    flags,				\
				    fields)				\
    GENERAL_ROOT_(storage_class, C_name, NULL);				\
    static field_descriptor_t fields[];			       		\
    __attribute__((constructor))					\
    static void UNIQ_IDENT(bind_record_)(void)				\
    {									\
        static rec_descriptor_t desc = {				\
	    &C_name,							\
	    flags,							\
	    scheme_name,						\
	    parent,							\
	    fields,							\
	    NULL							\
        };								\
        register_record(&desc);						\
    }									\
    static field_descriptor_t fields[]
    
typedef enum field_mutability {
    FM_MUTABLE,
    FM_IMMUTABLE,
    FM_END,
} field_mutability_t;

typedef struct field_descriptor {
    field_mutability_t        fd_mutability;
    const wchar_t            *fd_name;
} field_descriptor_t;

typedef struct rec_descriptor rec_descriptor_t;

struct rec_descriptor {
    obj_t                    *rd_root;
    rtd_flags_t               rd_flags;
    const wchar_t            *rd_name;
    obj_t                    *rd_parent;
    const field_descriptor_t *rd_fields;
    rec_descriptor_t         *rd_next;
};

extern void register_record(rec_descriptor_t *);
extern void init_records(void);

#endif /* !RECORD_INCLUDED */
