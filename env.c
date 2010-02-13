#include "env.h"

#include <assert.h>

#include "except.h"
#include "roots.h"
#include "types.h"

/*
 * An environment is currently implemented as a list of frames.  Each
 * frame is a list of bindings.  A binding is a primitive object with
 * three fields: a name (symbol), a value (object) and a mutability
 * flag.
 */

ROOT_CONSTRUCTOR(root_env)
{
    return make_env(EMPTY_LIST);
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

obj_t root_environment(void)
{
    return root_env;
}
