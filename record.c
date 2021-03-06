#include "record.h"

#include <assert.h>

#include "env.h"
#include "obj_boolean.h"
#include "obj_pair.h"
#include "obj_symbol.h"
#include "obj_uninit.h"
#include "obj_vector.h"

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
	assert(m == FM_MUTABLE || m == FM_IMMUTABLE);
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
    obj_t nsym = make_symbol_from_C_str(desc->rd_name);
    obj_t parent = desc->rd_parent ? *desc->rd_parent : FALSE_OBJ;
    obj_t rtd = make_rtd(desc->rd_flags,
			 nsym,
			 parent,
			 FALSE_OBJ,
			 FALSE_OBJ,
			 field_vec);
    *desc->rd_root = rtd;

    if (desc->rd_flags & RF_OPAQUE) {
	/* Do not create bindings for an opaque record type. */
	return;
    }

    /* Bind the rtd to name. */
    obj_t env = root_environment();
    env_bind(env, nsym, BT_LEXICAL, M_IMMUTABLE, rtd);

    /* Construct the constructor descriptor, constructor, predicate,
     * field accessors, and field mutators.
     */
    // XXX implement me.  Maybe the rest should be in Scheme.
}

void init_records(void)
{
    rec_descriptor_t *desc;
    bool done;
    do {
	done = true;
	for (desc = rec_descs; desc; desc = desc->rd_next) {
	    if (desc->rd_initialized)
		continue;
	    if (desc->rd_parent && is_uninitialized(*desc->rd_parent)) {
		done = false;
		continue;
	    } else {
		init_rec(desc);
		desc->rd_initialized = true;
	    }
	}
    } while (!done);
}
