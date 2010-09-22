#ifndef ENV_INCLUDED
#define ENV_INCLUDED

#include "obj_binding.h"
#include "obj_pair.h"
#include "obj.h"

#define OLD_ENV 1

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

#if OLD_ENV

static inline obj_t env_first_frame(obj_t env)
{
    return CAR(env);
}

static inline obj_t env_parent(obj_t env)
{
    return CDR(env);
}

#else

extern obj_t env_first_frame(obj_t env);

extern obj_t env_parent(obj_t env);

#endif

extern void  env_bind  (obj_t          env,
			obj_t          name,
			binding_type_t type,
			mutability_t   mutability,
			obj_t          value);
extern obj_t env_lookup(obj_t env, obj_t name);

extern obj_t env_make_ref(obj_t env, obj_t var);

extern obj_t env_ref_lookup(obj_t env_ref);

extern void  env_ref_set(obj_t env_ref, obj_t new_value);

extern obj_t frame_defines(obj_t frame, obj_t name);

extern obj_t root_environment(void);

#endif /* !ENV_INCLUDED */
