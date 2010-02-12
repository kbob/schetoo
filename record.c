#include "record.h"

#include "env.h"
#include "obj_boolean.h"
#include "obj_pair.h"
#include "obj_symbol.h"
#include "obj_vector.h"


DEFINE_EXTERN_RECORD_TYPE(testing123, L"toy-boat", NULL, 0) = {
    { FM_MUTABLE,   L"scupper" },
    { FM_IMMUTABLE, L"yardarm" },
    { FM_END }
};

static rec_descriptor_t *rec_descs;

void register_record(rec_descriptor_t *desc)
{
    desc->rd_next = rec_descs;
    rec_descs = desc;
}

static void init_rec(rec_descriptor_t *desc)
{
    /* Count the fields. */
    const field_descriptor_t *fields = desc->rd_fields;
    size_t i;
    for (i = 0; ; i++) {
	field_mutability_t m = fields[i].fd_mutability;
	if (m == FM_END)
	    break;
	ASSERT(m == FM_MUTABLE || m == FM_IMMUTABLE);
    }
    size_t field_count = i;

    obj_t field_vec = make_vector_uninitialized(field_count);
    for (i = 0; i < field_count; i++) {
	field_mutability_t m = fields[i].fd_mutability;
	obj_t msym = make_symbol_from_C_str(m == FM_MUTABLE ? L"mutable"
                                                            : L"immutable");
	obj_t nsym = make_symbol_from_C_str(fields[i].fd_name);
	vector_set(field_vec, i, CONS(msym, CONS(nsym, EMPTY_LIST)));
    }

    /* Construct the rtd. */
    obj_t env = root_environment();
    obj_t nsym = make_symbol_from_C_str(desc->rd_name);
    obj_t parent = FALSE_OBJ;
    if (desc->rd_parent) {
	obj_t psym = make_symbol_from_C_str(desc->rd_parent);
	parent = env_lookup(env, psym);
    }
    obj_t rtd = make_rtd(desc->rd_flags,
			 nsym,
			 parent,
			 FALSE_OBJ,
			 FALSE_OBJ,
			 field_vec);
    env_bind(env, nsym, BT_LEXICAL, M_IMMUTABLE, rtd);
    *desc->rd_root = rtd;

    /* Construct the constructor descriptor, constructor, predicate,
     * field accessors, and field mutators.
     */
    // XXX implement me.
}

void init_records(void)
{
    /* Reverse the descriptors so that each superclass is defined
     * before its subclasses.
     */

    rec_descriptor_t *forw = rec_descs;
    rec_descriptor_t *rev = NULL;
    while (forw) {
	rec_descriptor_t *next = forw->rd_next;
	forw->rd_next = rev;
	rev = forw;
	forw = next;
    }
    rec_descs = rev;

    rec_descriptor_t *desc;
    for (desc = rec_descs; desc; desc = desc->rd_next)
	init_rec(desc);
}
