#ifndef OBJ_ENV_REC_INCLUDED
#define OBJ_ENV_REC_INCLUDED

#include "mem_mixvec.h"

typedef enum env_type {
    ET_IMMUTABLE = -2,
    ET_CLOSED    = -1,
    ET_OPEN      =  0,
} env_type_t;

typedef enum env_ptrs {
    EP_PARENT,
    EP_FORMALS,
    EP_ACTUALS,
} env_ptrs_t;

extern        obj_t make_env_rec       (obj_t parent,
					int   nvar,
					obj_t formals,
					obj_t actuals);

static inline bool  is_env_rec         (obj_t obj);

static inline int   env_rec_nvar       (obj_t obj);

static inline int   env_rec_is_open    (obj_t env);
static inline int   env_rec_is_mutable (obj_t env);

static inline obj_t env_rec_parent     (obj_t env);
static inline obj_t env_rec_formals    (obj_t env);
static inline obj_t env_rec_actuals    (obj_t env);

static inline void  env_rec_set_nvar   (obj_t env, int   nvar);
static inline void  env_rec_set_formals(obj_t env, obj_t formals);
static inline void  env_rec_set_actuals(obj_t env, obj_t actuals);

OBJ_TYPE_PREDICATE(env_rec);

static inline int env_rec_nvar(obj_t env)
{
    return mixvec_1_3_get_int(env, 0);
}

static inline int env_rec_is_open(obj_t env)
{
    return env_rec_nvar(env) >= 0;
}

static inline int env_rec_is_mutable(obj_t env)
{
    return env_rec_nvar(env) != ET_IMMUTABLE;
}

static inline obj_t env_rec_parent(obj_t env)
{
    return mixvec_1_3_get_ptr(env, EP_PARENT);
}

static inline obj_t env_rec_formals(obj_t env)
{
    return mixvec_1_3_get_ptr(env, EP_FORMALS);
}

static inline obj_t env_rec_actuals(obj_t env)
{
    return mixvec_1_3_get_ptr(env, EP_ACTUALS);
}

static inline void env_rec_set_nvar(obj_t env, int nvar)
{
    mixvec_1_3_set_int(env, 0, nvar);
}

static inline void env_rec_set_formals(obj_t env, obj_t formals)
{
    mixvec_1_3_set_ptr(env, EP_FORMALS, formals);
}

static inline void env_rec_set_actuals(obj_t env, obj_t actuals)
{
    mixvec_1_3_set_ptr(env, EP_ACTUALS, actuals);
}

#endif /* !OBJ_ENV_REC_INCLUDED */
