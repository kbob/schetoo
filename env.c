#include "env.h"

#include "except.h"
#include "types.h"

/*
 * An environment is currently implemented as a list of frames.  Each
 * frame is a list of bindings.  A binding is a primitive object with
 * three fields: a name (symbol), a value (object) and a mutability
 * flag.
 */

void env_bind(obj_t env,
	      obj_t name,
	      binding_type_t type,
	      mutability_t mutability,
	      obj_t value)
{
    pair_set_car(env,
		 CONS(make_binding(name, type, mutability, value),
		      env_first_frame(env)));
}

obj_t env_lookup(obj_t env, obj_t var)
{
    /*
     * for frame in env:
     *     for binding in frame:
     *         if binding.name == var:
     *             return binding
     * assert False, 'unbound variable'
     */

    while (!is_null(env)) {
	obj_t frame = env_first_frame(env);
	while (!is_null(frame)) {
	    obj_t binding = pair_car(frame);
	    if (binding_name(binding) == var) {
		return binding;
	    }
	    frame = pair_cdr(frame);
	}
	env = env_parent(env);
    }
    raise(&undefined, var, "unbound variable");
}
