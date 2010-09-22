#include "env.h"

#include <assert.h>

#include "except.h"
#include "roots.h"
#include "types.h"

#if !OLD_ENV
    #include "record.h"
#endif

#if OLD_ENV

/*
 * An environment is currently implemented as a list of frames.  Each
 * frame is a list of bindings.  A binding is a primitive object with
 * three fields: a name (symbol), a value (object) and a mutability
 * flag.
 */

#endif

ROOT_CONSTRUCTOR(root_env)
{
    return make_env(EMPTY_LIST);
}

#if OLD_ENV

obj_t make_closed_env(obj_t parent, obj_t formals, obj_t actuals)
{
    obj_t env = make_env(parent);
    obj_t formal;
    obj_t actual;
    while (!is_null(formals) || !is_null(actuals)) {
	CHECK(!is_null(formals), "too many arguments");
	if (is_pair(formals)) {
	    CHECK(!is_null(actuals), "not enough arguments");
	    formal  = CAR(formals);
	    formals = CDR(formals);
	    actual  = CAR(actuals);
	    actuals = CDR(actuals);
	} else {
	    formal  = formals;
	    actual  = actuals;
	    formals = actuals = EMPTY_LIST;
	}
	env_bind(env, formal, BT_LEXICAL, M_MUTABLE, actual);
    }
    return env;
}

#else

// an env is a record.  The fields are parent, flags, formals, and values.
//
// There are three kinds of env's.  Most restrictive is an immutable env.
// Second is mutable but non-extensible.  Third is mutable-extensible.a
//
// In an inextensible env, the formals are lambda's arglist.  In an
// extensible env, the formals are a vector.

typedef enum env_type {
    ET_OPEN,
    ET_IMMUTABLE,
    ET_INEXTENSIBLE
} env_type_t;

DEFINE_STATIC_RECORD_TYPE(env, L"env", NULL, RF_SEALED | RF_OPAQUE) = {
    { FM_IMMUTABLE, L"parent"   },
    { FM_MUTABLE,   L"flags"    },
    { FM_MUTABLE,   L"formals"  },
    { FM_MUTABLE,   L"bindings" },
    { FM_END }
};

obj_t make_env(obj_t parent)
{
    // Always create an open env and lock it afterwards.

    return MAKE_RECORD(env,
		       parent,
		       ET_OPEN,
		       make_uninitialized(),
		       make_uninitialized());
}

#endif

#if OLD_ENV

void env_bind(obj_t env,
	      obj_t name,
	      binding_type_t type,
	      mutability_t mutability,
	      obj_t value)
{
    assert(is_symbol(name));
    pair_set_car(env,
		 CONS(make_binding(name, type, mutability, value),
		      env_first_frame(env)));
}

#else

void env_bind(obj_t env,
	      obj_t name,
	      binding_type_t type,
	      mutability_t mutability,
	      obj_t value)
{
    assert(is_symbol(name));
    if (is_uninitialized
}

#endif

obj_t frame_lookup(obj_t frame, obj_t var)
{
    while (!is_null(frame)) {
	obj_t binding = pair_car(frame);
	if (binding_name(binding) == var) {
	    return binding;
	}
	frame = pair_cdr(frame);
    }
    return FALSE_OBJ;
}

obj_t env_lookup(obj_t env, obj_t var)
{
    while (!is_null(env)) {
	obj_t binding = frame_lookup(env_first_frame(env), var);
	if (binding != FALSE_OBJ)
	    return binding;
	env = env_parent(env);
    }
    THROW(&undefined, "unbound variable");
}

obj_t env_make_ref(obj_t env, obj_t var)
{
    return env_lookup(env, var);
}

obj_t env_ref_lookup(obj_t ref)
{
    return binding_value(ref);
}

void env_ref_set(obj_t ref, obj_t new_value)
{
    binding_set_value(ref, new_value);
}

obj_t root_environment(void)
{
    return root_env;
}
