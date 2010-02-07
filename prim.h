#ifndef PRIM_INCLUDED
#define PRIM_INCLUDED

#include "interval.h"
#include "obj.h"
#include "obj_cont.h"
#include "obj_proc.h"
#include "uniq.h"

/* abbreviations */
#define DEFINE_PROC                DEFINE_ANONYMOUS_PROC
#define DEFINE_RAW_PROC            DEFINE_ANONYMOUS_RAW_PROC
#define DEFINE_SPECIAL_FORM        DEFINE_ANONYMOUS_SPECIAL_FORM
#define DEFINE_COOKED_SPECIAL_FORM DEFINE_ANONYMOUS_COOKED_SPECIAL_FORM

#define DEFINE_EXTERN_PROC(C_name, scheme_name, args)			\
    DEFINE_GENERAL_PRIM_(extern,					\
			C_name,						\
			scheme_name,					\
			obj_t,						\
			args,						\
			create_proc)

#define DEFINE_STATIC_PROC(C_name, scheme_name, args)			\
    DEFINE_GENERAL_PRIM_(static,					\
			C_name,						\
			scheme_name,					\
			obj_t,						\
			args,						\
			create_proc)

#define DEFINE_ANONYMOUS_PROC(scheme_name, args)			\
    DEFINE_GENERAL_PRIM_(static,					\
			UNIQ_IDENT(anonymous_),				\
			scheme_name,					\
			obj_t,						\
			args,						\
			create_proc)

#define DEFINE_EXTERN_RAW_PROC(C_name, scheme_name)			\
    DEFINE_GENERAL_PRIM_(extern,					\
			 C_name,					\
			 scheme_name,					\
			 cv_t,						\
			 0,						\
			 create_raw_proc)

#define DEFINE_STATIC_RAW_PROC(C_name, scheme_name)			\
    DEFINE_GENERAL_PRIM_(static,					\
			 C_name,					\
			 scheme_name,					\
			 cv_t,						\
			 0,						\
			 create_raw_proc)

#define DEFINE_ANONYMOUS_RAW_PROC(scheme_name)				\
    DEFINE_GENERAL_PRIM_(static,					\
                         UNIQ_IDENT(anonymous_),			\
                         scheme_name,					\
			 cv_t,						\
			 0,						\
			 create_raw_proc)

#define DEFINE_EXTERN_SPECIAL_FORM(C_name, scheme_name)			\
    DEFINE_GENERAL_PRIM_(extern,					\
			 C_name,					\
			 scheme_name,					\
	 		 cv_t,						\
			 0,						\
			 create_special_form)

#define DEFINE_STATIC_SPECIAL_FORM(C_name, scheme_name)			\
    DEFINE_GENERAL_PRIM_(static,					\
			 C_name,					\
			 scheme_name,					\
	 		 cv_t,						\
			 0,						\
			 create_special_form)

#define DEFINE_ANONYMOUS_SPECIAL_FORM(scheme_name)			\
    DEFINE_GENERAL_PRIM_(static,					\
                         UNIQ_IDENT(anonymous_),			\
                         scheme_name,					\
			 cv_t,						\
			 0,						\
			 create_special_form)

#define DEFINE_EXTERN_COOKED_SPECIAL_FORM(C_name, scheme_name, args)	\
    DEFINE_GENERAL_PRIM_(extern,					\
			C_name,						\
			scheme_name,					\
			obj_t,						\
			args,						\
			create_cooked_special_form)

#define DEFINE_STATIC_COOKED_SPECIAL_FORM(C_name, scheme_name, args)	\
    DEFINE_GENERAL_PRIM_(static,					\
			C_name,						\
			scheme_name,					\
			obj_t,						\
			args,						\
			create_cooked_special_form)

#define DEFINE_ANONYMOUS_COOKED_SPECIAL_FORM(scheme_name, args)		\
    DEFINE_GENERAL_PRIM_(static,					\
			UNIQ_IDENT(anonymous_),				\
			scheme_name,					\
			obj_t,						\
			args,						\
			create_cooked_special_form)

#define ALIAS_NAME(old_name, new_name)					\
    __attribute__((constructor))					\
    static void UNIQ_IDENT(alias_proc_)(void)				\
    {									\
	static alias_descriptor_t desc = {				\
	    old_name,							\
	    new_name,							\
	    NULL							\
	};								\
	register_alias(&desc);						\
    }

#define DEFINE_GENERAL_PRIM_(storage_class,				\
			     C_name,					\
			     scheme_name,				\
			     return_type,				\
			     arg_range,					\
			     binder)					\
    storage_class return_type C_name();					\
    __attribute__((constructor))					\
    static void UNIQ_IDENT(bind_prim_)(void)				\
    {									\
	static prim_descriptor_t desc = {				\
	    (obj_t (*)())C_name,					\
	    scheme_name,						\
	    COMPILE_INTERVAL(arg_range),				\
	    binder,							\
	    NULL							\
	};								\
	register_prim(&desc);						\
    }									\
    storage_class return_type C_name

typedef struct prim_descriptor  prim_descriptor_t;
typedef struct alias_descriptor alias_descriptor_t;
typedef obj_t                   creator_t(const prim_descriptor_t *);

struct prim_descriptor {
    obj_t               (*pd_prim)();
    const wchar_t        *pd_name;
    interval_t            pd_arg_range;
    creator_t            *pd_creator;
    prim_descriptor_t    *pd_next;
};

struct alias_descriptor {
    const wchar_t        *ad_old_name;
    const wchar_t        *ad_new_name;
    alias_descriptor_t   *ad_next;
};

extern void  register_prim             (prim_descriptor_t *);
extern void  register_primitives       (void);
extern void  register_alias            (alias_descriptor_t *);

extern obj_t create_proc               (const prim_descriptor_t *);
extern obj_t create_raw_proc           (const prim_descriptor_t *);
extern obj_t create_special_form       (const prim_descriptor_t *);
extern obj_t create_cooked_special_form(const prim_descriptor_t *);

extern obj_t apply_proc                (obj_t proc, obj_t arg_list);

#endif /* !PROC_INCLUDED */
