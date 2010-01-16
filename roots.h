#ifndef ROOTS_INCLUDED
#define ROOTS_INCLUDED

#include "mem.h"
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

#define AUTO_ROOT(name, value)						\
    obj_t name = (value);						\
    PUSH_ROOT(name);

#define PUSH_ROOT(name)							\
    root_descriptor_t UNIQ_IDENT(auto_root_) = {			\
	L ## #name,							\
	__func__,							\
	&name,								\
	NULL,								\
	NULL								\
    };									\
    push_root(&UNIQ_IDENT(auto_root_));

#define POP_ROOT(name) \
    pop_root(L ## #name)

#define POP_FUNCTION_ROOTS() pop_function_roots(__func__)

#define GENERAL_ROOT_CONSTRUCTOR_(storage_class, name)			\
    DECLARE_ROOT_CONSTRUCTOR_(name);					\
    GENERAL_ROOT_(storage_class, name, init_root_##name)		\
    DECLARE_ROOT_CONSTRUCTOR_(name)
    
#define DECLARE_ROOT_CONSTRUCTOR_(name) \
    static obj_t init_root_##name(void)

#define GENERAL_ROOT_(storage_class, name, init) 			\
    storage_class obj_t name = NIL;					\
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
typedef obj_t root_constructor_t(void);

struct root_descriptor {
    const wchar_t      *rd_name;
    const char         *rd_func;
    obj_t              *rd_root;
    root_constructor_t *rd_init;
    root_descriptor_t  *rd_next;
};

extern void record_static_root(root_descriptor_t *);
extern void push_root(root_descriptor_t *);
extern void pop_root(const wchar_t *);
extern void pop_function_roots(const char *func_name);
extern root_descriptor_t *get_thread_roots(void);
extern void init_roots(void);

#endif /* !ROOTS_INCLUDED */
