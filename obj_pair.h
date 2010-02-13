#ifndef OBJ_PAIR_INCLUDED
#define OBJ_PAIR_INCLUDED

#include "obj.h"
#include "mem_fixvec.h"

extern obj_t        make_pair   (obj_t car, obj_t cdr);

static inline bool  is_pair     (obj_t);
static inline obj_t pair_car    (obj_t);
static inline obj_t pair_cdr    (obj_t);
extern void         pair_set_car(obj_t pair, obj_t car);
extern void         pair_set_cdr(obj_t pair, obj_t cdr);

OBJ_TYPE_PREDICATE(pair)		// bool is_pair(obj_t);

static inline obj_t pair_car(obj_t pair)
{
    CHECK_OBJ(pair);
    CHECK(is_pair(pair), "must be pair", pair);
    return fixvec2_get_ptr(pair, 0);
}

static inline obj_t pair_cdr(obj_t pair)
{
    CHECK_OBJ(pair);
    CHECK(is_pair(pair), "must be pair", pair);
    return fixvec2_get_ptr(pair, 1);
}

#define CONS(car, cdr) (make_pair((car), (cdr)))

#define CAR(pair)      (pair_car((pair)))
#define CDR(pair)      (pair_cdr((pair)))

#define CAAR(pair)     (CAR(CAR((pair))))
#define CADR(pair)     (CAR(CDR((pair))))
#define CDAR(pair)     (CDR(CAR((pair))))
#define CDDR(pair)     (CDR(CDR((pair))))

#define CAAAR(pair)    (CAR(CAR(CAR((pair)))))
#define CAADR(pair)    (CAR(CAR(CDR((pair)))))
#define CADAR(pair)    (CAR(CDR(CAR((pair)))))
#define CADDR(pair)    (CAR(CDR(CDR((pair)))))
#define CDAAR(pair)    (CDR(CAR(CAR((pair)))))
#define CDADR(pair)    (CDR(CAR(CDR((pair)))))
#define CDDAR(pair)    (CDR(CDR(CAR((pair)))))
#define CDDDR(pair)    (CDR(CDR(CDR((pair)))))

#define CAAAAR(pair)   (CAR(CAR(CAR(CAR((pair))))))
#define CAAADR(pair)   (CAR(CAR(CAR(CDR((pair))))))
#define CAADAR(pair)   (CAR(CAR(CDR(CAR((pair))))))
#define CAADDR(pair)   (CAR(CAR(CDR(CDR((pair))))))
#define CADAAR(pair)   (CAR(CDR(CAR(CAR((pair))))))
#define CADADR(pair)   (CAR(CDR(CAR(CDR((pair))))))
#define CADDAR(pair)   (CAR(CDR(CDR(CAR((pair))))))
#define CADDDR(pair)   (CAR(CDR(CDR(CDR((pair))))))
#define CDAAAR(pair)   (CDR(CAR(CAR(CAR((pair))))))
#define CDAADR(pair)   (CDR(CAR(CAR(CDR((pair))))))
#define CDADAR(pair)   (CDR(CAR(CDR(CAR((pair))))))
#define CDADDR(pair)   (CDR(CAR(CDR(CDR((pair))))))
#define CDDAAR(pair)   (CDR(CDR(CAR(CAR((pair))))))
#define CDDADR(pair)   (CDR(CDR(CAR(CDR((pair))))))
#define CDDDAR(pair)   (CDR(CDR(CDR(CAR((pair))))))
#define CDDDDR(pair)   (CDR(CDR(CDR(CDR((pair))))))

#endif /* !OBJ_PAIR_INCLUDED */
