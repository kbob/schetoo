#ifndef OBJ_PROCEDURE_INCLUDED
#define OBJ_PROCEDURE_INCLUDED

#include "except.h"
#include "mem.h"

typedef enum proc_flags {
    PF_COMPILED_C   = 1 << 0,
    PF_SPECIAL_FORM = 1 << 1,
} proc_flags_t;

typedef obj_t (*C_procedure_t)(obj_t value);

typedef struct proc_obj {
    heap_object_t proc_header;
    word_t        proc_flags;
    obj_t         proc_args;
    obj_t         proc_env;
    union {
	obj_t         pu_body;
	C_procedure_t pu_code;
    }             proc_u;
} proc_obj_t;

OBJ_TYPE_PREDICATE(procedure)		// bool is_procedure(obj_t);

extern obj_t make_procedure               (obj_t code,
					   obj_t arglist,
					   obj_t env);
extern obj_t make_C_procedure		  (C_procedure_t code,
					   obj_t arglist,
					   obj_t env);
extern obj_t make_special_form_procedure  (obj_t code,
					   obj_t arglist,
					   obj_t env);
extern obj_t make_C_special_form_procedure(C_procedure_t code,
					   obj_t arglist,
					   obj_t env);

static inline bool procedure_is_C(obj_t proc)
{
    CHECK(is_procedure(proc), NULL, "must be procedure", proc);
    return ((proc_obj_t *)proc)->proc_flags & PF_COMPILED_C ? true : false;
}

static inline bool procedure_is_special_form(obj_t proc)
{
    CHECK(is_procedure(proc), NULL, "must be procedure", proc);
    return ((proc_obj_t *)proc)->proc_flags & PF_SPECIAL_FORM ? true : false;
}

static inline obj_t procedure_body(obj_t proc)
{
    CHECK(is_procedure(proc), NULL, "must be procedure", proc);
    CHECK(!procedure_is_C(proc), NULL, "must be Scheme procedure", proc);
    return ((proc_obj_t *)proc)->proc_u.pu_body;
}

static inline C_procedure_t procedure_code(obj_t proc)
{
    CHECK(is_procedure(proc), NULL, "must be procedure", proc);
    CHECK(procedure_is_C(proc), NULL, "must be C procedure", proc);
    return ((proc_obj_t *)proc)->proc_u.pu_code;
}

static inline obj_t procedure_args(obj_t proc)
{
    CHECK(is_procedure(proc), NULL, "must be procedure", proc);
    CHECK(!procedure_is_C(proc), NULL, "must be Scheme procedure", proc);
    return ((proc_obj_t *)proc)->proc_args;
}

static inline obj_t procedure_env(obj_t proc)
{
    CHECK(is_procedure(proc), NULL, "must be procedure", proc);
    return ((proc_obj_t *)proc)->proc_env;
}

#endif /* !OBJ_PROCEDURE_INCLUDED */
