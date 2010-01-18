#ifndef ROOTS_INCLUDED
#define ROOTS_INCLUDED

#include "obj.h"
#include "uniq.h"

#define ROOT             STATIC_ROOT
#define ROOT_CONSTRUCTOR STATIC_ROOT_CONSTRUCTOR

#define EXTERN_ROOT(name) GENERAL_ROOT_(, name, NULL)
#define STATIC_ROOT(name) GENERAL_ROOT_(static, name, NULL)

#define EXTERN_ROOT_CONSTRUCTOR(name) GENERAL_ROOT_CONSTRUCTOR_(, name)
#define STATIC_ROOT_CONSTRUCTOR(name) GENERAL_ROOT_CONSTRUCTOR_(static, name)

#define THREAD_ROOT        THREAD_STATIC_ROOT
#define THREAD_EXTERN_ROOT EXTERN_ROOT
#define THREAD_STATIC_ROOT STATIC_ROOT

#define GENERAL_ROOT_CONSTRUCTOR_(storage_class, name)			\
    DECLARE_ROOT_CONSTRUCTOR_(name);					\
    GENERAL_ROOT_(storage_class, name, init_root_##name)		\
    DECLARE_ROOT_CONSTRUCTOR_(name)
    
#define DECLARE_ROOT_CONSTRUCTOR_(name) \
    static obj_t init_root_##name(void)

#define GENERAL_ROOT_(storage_class, name, init) 			\
    storage_class obj_t name = EMPTY_LIST;				\
    __attribute__((constructor)) 					\
    static void mem_record_root_ ## name(void) 				\
    { 									\
	static root_descriptor_t desc = {				\
            L ## #name,							\
	    NULL,							\
	    &name,							\
	    (init),							\
            NULL							\
        };								\
        record_static_root(&desc);					\
    }

typedef struct root_descriptor root_descriptor_t;
typedef obj_t                  root_constructor_t(void);

struct root_descriptor {
    const wchar_t      *rd_name;
    const char         *rd_func;
    obj_t              *rd_root;
    root_constructor_t *rd_init;
    root_descriptor_t  *rd_next;
};

extern void               record_static_root(root_descriptor_t *);
extern root_descriptor_t *get_thread_roots(void);
extern void               init_roots(void);

#endif /* !ROOTS_INCLUDED */
