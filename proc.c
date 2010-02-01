#include "proc.h"

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "env.h"
#include "obj_symbol.h"
#include "roots.h"

static proc_descriptor_t *proc_descs;
static alias_descriptor_t *alias_descs;

extern obj_t create_proc(const proc_descriptor_t *desc)
{
    return make_C_procedure(desc->pd_proc,
			    desc->pd_arg_range,
			    root_environment());
}

extern void register_proc(proc_descriptor_t *desc)
{
#ifndef NDEBUG
    proc_descriptor_t *p = proc_descs;
    while (p) {
	if (!wcscmp(p->pd_name, desc->pd_name)) {
	    fprintf(stderr, "duplicate proc name \"%ls\"\n", p->pd_name);
	    abort();
	}
	p = p->pd_next;
    }
#endif
    desc->pd_next = proc_descs;
    proc_descs = desc;
}

extern void register_procs(void)
{
    obj_t root_env = root_environment();

    {
	const proc_descriptor_t *desc;

	for (desc = proc_descs; desc; desc = desc->pd_next) {
	    obj_t symbol = make_symbol_from_C_str(desc->pd_name);
	    obj_t value = (*desc->pd_creator)(desc);
	    env_bind(root_env, symbol, BT_LEXICAL, M_MUTABLE, value);
	    proc_descs = desc->pd_next;
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

