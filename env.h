#ifndef ENV_INCLUDED
#define ENV_INCLUDED

#include "obj_binding.h"
#include "obj_pair.h"
#include "obj.h"

/* environments */
static inline obj_t make_env(obj_t parent)
{
    return CONS(EMPTY_LIST, parent);
}

static inline obj_t env_first_frame(obj_t env)
{
    return CAR(env);
}

static inline obj_t env_parent(obj_t env)
{
    return CDR(env);
}

extern void  env_bind  (obj_t          env,
			obj_t          name,
			binding_type_t type,
			mutability_t   mutability,
			obj_t          value);
extern obj_t env_lookup(obj_t env, obj_t name);

extern obj_t frame_defines(obj_t frame, obj_t name);

extern obj_t root_environment(void);

#endif /* !ENV_INCLUDED */
