#ifndef OBJ_PAIR_INCLUDED
#define OBJ_PAIR_INCLUDED

#include "mem.h"

extern obj_t make_pair   (obj_t car, obj_t cdr);
extern bool  is_pair     (obj_t);
extern obj_t pair_car    (obj_t);
extern obj_t pair_cdr    (obj_t);
extern void  pair_set_car(obj_t pair, obj_t car);
extern void  pair_set_cdr(obj_t pair, obj_t cdr);

static inline obj_t pair_caar(obj_t pair)
{
    return pair_car(pair_car(pair));
}

static inline obj_t pair_cadr(obj_t pair)
{
    return pair_car(pair_cdr(pair));
}

static inline obj_t pair_cdar(obj_t pair)
{
    return pair_cdr(pair_car(pair));
}

static inline obj_t pair_cddr(obj_t pair)
{
    return pair_cdr(pair_cdr(pair));
}

static inline obj_t pair_cadar(obj_t pair)
{
    return pair_car(pair_cdr(pair_car(pair)));
}

static inline obj_t pair_caddr(obj_t pair)
{
    return pair_car(pair_cdr(pair_cdr(pair)));
}

static inline obj_t pair_cadddr(obj_t pair)
{
    return pair_car(pair_cdr(pair_cdr(pair_cdr(pair))));
}

static inline obj_t pair_cdaddr(obj_t pair)
{
    return pair_cdr(pair_car(pair_cdr(pair_cdr(pair))));
}

static inline obj_t pair_cddddr(obj_t pair)
{
    return pair_cdr(pair_cdr(pair_cdr(pair_cdr(pair))));
}

static inline obj_t pair_cdadddr(obj_t pair)
{
    return pair_cdr(pair_car(pair_cdr(pair_cdr(pair_cdr(pair)))));
}


#endif /* !OBJ_PAIR_INCLUDED */
