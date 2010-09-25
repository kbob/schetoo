#include "env.h"

#include <assert.h>

#include "except.h"
#include "roots.h"
#include "types.h"

#if !OLD_ENV
    #include "list.h"
    #include "obj_env_rec.h"
    #include "record.h"
#endif

#if OLD_ENV

/*
 * An environment is currently implemented as a list of frames.  Each
 * frame is a list of bindings.  A binding is a primitive object with
 * three fields: a name (symbol), a value (object) and a mutability
 * flag.
 */

#else

/*
 * An environment is implemented as an env_rec object.  An env_rec
 * has four fields: parent, nvar, formals, and actuals.
 *
 *  parent is the environment's parent (lexically enclosing) environment.
 *
 *  nvar is either the number of variables or one of the special
 *  values ET_CLOSED or ET_IMMUTABLE.  A closed environment is one
 *  that can't have more variables added after it is created.  An
 *  immutable environment is one whose bindings and parents' bindings
 *  can not be changed.  An immutable environment is also closed.
 *
 *  formals is used differently in open and closed environments.
 *  In an open environment, formals is a vector of variables (symbols).
 *  In a close environment, formals is a possibly irregular list of
 *  variables identical to lambda's argument list.
 *
 *  actuals is a vector of bound values.
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

obj_t make_env(obj_t parent)
{
    return make_env_rec(parent,
			ET_OPEN,
			make_uninitialized(),
			make_uninitialized());
}

obj_t make_closed_env(obj_t parent, obj_t formals, obj_t actuals)
{
    int nvar = irregular_list_length(formals);
    obj_t new_formals = make_vector_uninitialized(nvar);
    obj_t new_actuals = make_vector_uninitialized(nvar);
    obj_t formal;
    obj_t actual;
    size_t pos = 0;
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
	vector_set(new_formals, pos, formal);
	vector_set(new_actuals, pos, actual);
	pos++;
    }
    return make_env_rec(parent, nvar, new_formals, new_actuals);
}

#endif

#if OLD_ENV

static inline obj_t env_first_frame(obj_t env)
{
    return CAR(env);
}

static inline obj_t env_parent(obj_t env)
{
    return CDR(env);
}

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

obj_t grow_vector(obj_t vec, size_t new_size)
{
    size_t old_size = is_vector(vec) ? vector_len(vec) : 0;
    assert(new_size > old_size);
    obj_t new_vec = make_vector_uninitialized(new_size);
    int i;
    for (i = 0; i < old_size; i++)
	vector_set(new_vec, i, vector_ref(vec, i));
    for ( ; i < new_size; i++)
	vector_set(new_vec, i, make_uninitialized());
    return new_vec;
}

void env_bind(obj_t env,
	      obj_t name,
	      binding_type_t type,
	      mutability_t mutability,
	      obj_t value)
{
    assert(is_symbol(name));
    word_t nvar = env_rec_nvar(env);
    CHECK(env_rec_is_open(env), "environment is closed");
    if (nvar == 0 || nvar >= vector_len(env_rec_formals(env))) {
	word_t nnvar = nvar ? 2 * nvar : 2;
	obj_t new_formals = grow_vector(env_rec_formals(env), nnvar);
	obj_t new_actuals = grow_vector(env_rec_actuals(env), nnvar);
	env_rec_set_formals(env, new_formals);
	env_rec_set_actuals(env, new_actuals);
    }
    vector_set_nc(env_rec_formals(env), nvar, name);
    vector_set_nc(env_rec_actuals(env), nvar, value);
    env_rec_set_nvar(env, nvar + 1);
}

#endif

#if OLD_ENV

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
	    return binding_value(binding);
	env = env_parent(env);
    }
    THROW(&undefined, "unbound variable");
}

void env_set(obj_t env, obj_t name, obj_t value)
{
    while (!is_null(env)) {
	obj_t binding = frame_lookup(env_first_frame(env), name);
	if (binding != FALSE_OBJ) {
	    binding_set_value(binding, value);
	    return;
	}
	env = env_parent(env);
    }
    THROW(&undefined, "unbound variable");
}

#else

int formals_lookup(obj_t formals, word_t nvar, obj_t var)
{
    if (is_vector(formals)) {
	for (int i = 0; i < nvar; i++)
	    if (var == vector_ref(formals, i))
		return i;
    } else {
	int pos = 0;
	while (is_pair(formals)) {
	    if (CAR(formals) == var)
		return pos;
	    pos++;
	    formals = CDR(formals);
	}
	if (!is_null(formals) && formals == var)
	    return pos;
    }
    return -1;
}

obj_t env_lookup(obj_t env, obj_t name)
{
    while (!is_null(env)) {
	int pos = formals_lookup(env_rec_formals(env), env_rec_nvar(env), name);
	if (pos >= 0)
	    return vector_ref(env_rec_actuals(env), pos);
	env = env_rec_parent(env);
    }
    THROW(&undefined, "unbound variable");
}

void env_set(obj_t env, obj_t name, obj_t value)
{
    bool mutable = true;
    while (!is_null(env)) {
	if (!env_rec_is_mutable(env))
	    mutable = false;
	int pos = formals_lookup(env_rec_formals(env), env_rec_nvar(env), name);
	if (pos >= 0) {
	    CHECK(mutable, "must be mutable");
	    vector_set(env_rec_actuals(env), pos, value);
	    return;
	}
	env = env_rec_parent(env);
    }
    THROW(&undefined, "unbound variable");
}

#endif

#if !OLD_ENV

DEFINE_EXTERN_RECORD_TYPE(env_ref, L"env-ref", NULL, RF_SEALED | RF_OPAQUE) = {
    { FM_IMMUTABLE, L"env" },
    { FM_IMMUTABLE, L"index" },
    { FM_END }
};

obj_t env_make_ref(obj_t env, obj_t var)
{
    while (!is_null(env)) {
	int pos = formals_lookup(env_rec_formals(env), env_rec_nvar(env), var);
	if (pos >= 0)
	    return MAKE_RECORD(env_ref, env, make_fixnum(pos));
	env = env_rec_parent(env);
    }
    THROW(&undefined, "unbound variable");
}

obj_t env_ref_lookup(obj_t env_ref)
{
    obj_t env = record_get_field(env_ref, 0);
    obj_t actuals = env_rec_actuals(env);
    int pos = fixnum_value(record_get_field(env_ref, 1));
    return vector_ref(actuals, pos);
}

void env_ref_set(obj_t env_ref, obj_t new_value)
{
    obj_t env = record_get_field(env_ref, 0);
    obj_t actuals = env_rec_actuals(env);
    int pos = fixnum_value(record_get_field(env_ref, 1));
    vector_set(actuals, pos, new_value);
}

#endif

obj_t root_environment(void)
{
    return root_env;
}
