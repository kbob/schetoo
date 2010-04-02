#include "prim.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "env.h"
#include "list.h"
#include "obj_null.h"
#include "obj_symbol.h"

static prim_descriptor_t *prim_descs;
static alias_descriptor_t *alias_descs;

extern obj_t create_proc(const prim_descriptor_t *desc)
{
    return make_C_procedure(desc->pd_code,
			    make_symbol_from_C_str(desc->pd_name),
			    desc->pd_arg_range,
			    root_environment());
}

extern obj_t create_raw_proc(const prim_descriptor_t *desc)
{
    return make_raw_procedure((cont_proc_t)desc->pd_code,
			      make_symbol_from_C_str(desc->pd_name),
			      root_environment());
}

extern obj_t create_special_form(const prim_descriptor_t *desc)
{
    obj_t name = make_symbol_from_C_str(desc->pd_name);
    return make_raw_special_form_procedure((cont_proc_t)desc->pd_code,
					   name,
					   root_environment());
}

extern obj_t create_cooked_special_form(const prim_descriptor_t *desc)
{
    return make_C_special_form_procedure(desc->pd_code,
					 make_symbol_from_C_str(desc->pd_name),
					 desc->pd_arg_range,
					 root_environment());
}

extern void register_prim(prim_descriptor_t *desc)
{
#ifndef NDEBUG
    prim_descriptor_t *p = prim_descs;
    while (p) {
	if (p->pd_name && desc->pd_name && !wcscmp(p->pd_name, desc->pd_name)) {
	    fprintf(stderr, "duplicate prim name \"%ls\"\n", p->pd_name);
	    abort();
	}
	p = p->pd_next;
    }
#endif
    desc->pd_next = prim_descs;
    prim_descs = desc;
}

extern void register_alias(alias_descriptor_t *desc)
{
    desc->ad_next = alias_descs;
    alias_descs = desc;
}

extern void register_primitives(void)
{
    obj_t root_env = root_environment();

    {
	const prim_descriptor_t *desc;

	for (desc = prim_descs; desc; desc = desc->pd_next) {
	    if (desc->pd_name) {
		obj_t symbol = make_symbol_from_C_str(desc->pd_name);
		obj_t value = (*desc->pd_creator)(desc);
		env_bind(root_env, symbol, BT_LEXICAL, M_MUTABLE, value);
		prim_descs = desc->pd_next;
	    }
	}
    }
    {
	const alias_descriptor_t *desc;

	for (desc = alias_descs; desc; desc = desc->ad_next) {
	    obj_t old_sym = make_symbol_from_C_str(desc->ad_old_name);
	    obj_t binding = env_lookup(root_env, old_sym);
	    obj_t value = binding_value(binding);
	    obj_t new_sym = make_symbol_from_C_str(desc->ad_new_name);
	    env_bind(root_env, new_sym, BT_LEXICAL, M_MUTABLE, value);
	}
    }
}

static inline obj_t opt_arg(size_t pos, obj_t arg_list)
{
    while (--pos > 0) {
	if (is_null(arg_list))
	    return MISSING_ARG;
	arg_list = CDR(arg_list);
    }
    return CAR(arg_list);
}
