#ifndef ENV_INCLUDED
#define ENV_INCLUDED

#define OLD_ENV 0

#include "obj_binding.h"
#include "obj_pair.h"
#include "obj.h"

#if OLD_ENV

/* environments */
static inline obj_t make_env(obj_t parent)
{
    return CONS(EMPTY_LIST, parent);
}

#else

extern obj_t make_env(obj_t parent);

#endif

extern obj_t make_closed_env(obj_t parent, obj_t formals, obj_t actuals);

extern void  env_bind  (obj_t          env,
			obj_t          name,
			binding_type_t type,
			mutability_t   mutability,
			obj_t          value);

extern obj_t env_lookup(obj_t env, obj_t name);

extern void  env_set(obj_t env, obj_t name, obj_t value);

#if !OLD_ENV
    extern obj_t env_ref;

    extern obj_t env_make_ref(obj_t env, obj_t var);

    extern obj_t env_ref_lookup(obj_t env_ref);

    extern void  env_ref_set(obj_t env_ref, obj_t new_value);
#endif

extern obj_t root_environment(void);

#endif /* !ENV_INCLUDED */
