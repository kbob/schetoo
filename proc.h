#ifndef PROC_INCLUDED
#define PROC_INCLUDED

#include "interval.h"
#include "obj.h"
#include "obj_procedure.h"
#include "uniq.h"

/* abbreviations */
#define DEFINE_PROC         DEFINE_ANONYMOUS_PROC
#define DEFINE_SPECIAL_FORM DEFINE_ANONYMOUS_SPECIAL_FORM

#define DEFINE_EXTERN_PROC(C_name, scheme_name, args)			\
    DEFINE_GENERAL_PROC_(extern,					\
			 C_name,					\
			 scheme_name,					\
			 args,						\
			 create_proc)

#define DEFINE_STATIC_PROC(C_name, scheme_name, args)			\
    DEFINE_GENERAL_PROC_(static,					\
			 C_name,					\
			 scheme_name,					\
			 args,						\
			 create_proc)

#define DEFINE_ANONYMOUS_PROC(scheme_name, args)			\
    DEFINE_GENERAL_PROC_(static,					\
                         UNIQ_IDENT(anonymous_),			\
                         scheme_name,					\
			 args,						\
			 create_proc)

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

#define DEFINE_GENERAL_PROC_(storage_class,				\
			     C_name,					\
			     scheme_name,				\
			     arg_range,					\
			     binder)					\
    storage_class obj_t C_name();					\
    __attribute__((constructor))					\
    static void UNIQ_IDENT(bind_proc_)(void)				\
    {									\
	static proc_descriptor_t desc = {				\
	    C_name,							\
	    scheme_name,						\
	    COMPILE_INTERVAL(arg_range),				\
	    binder,							\
	    NULL							\
	};								\
	register_proc(&desc);						\
    }									\
    storage_class obj_t C_name

typedef struct proc_descriptor  proc_descriptor_t;
typedef struct alias_descriptor alias_descriptor_t;
typedef obj_t                   creator_t(const proc_descriptor_t *);

struct proc_descriptor {
    obj_t               (*pd_proc)();
    const wchar_t        *pd_name;
    interval_t            pd_arg_range;
    creator_t            *pd_creator;
    proc_descriptor_t    *pd_next;
};

struct alias_descriptor {
    const wchar_t        *ad_old_name;
    const wchar_t        *ad_new_name;
    alias_descriptor_t   *ad_next;
};

extern void  register_proc      (proc_descriptor_t *);
extern void  register_procs     (void);
extern void  register_alias     (alias_descriptor_t *);

extern obj_t create_proc        (const proc_descriptor_t *);
extern obj_t create_special_form(const proc_descriptor_t *);

#endif /* !PROC_INCLUDED */
