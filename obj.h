#ifndef OBJ_INCLUDED
#define OBJ_INCLUDED

#include <stdint.h>
#include <stddef.h>

#include "bool.h"

/*
 * Globally visible memory manager definitions.
 */

/*
 * See the long comment in mem.h for the meanings of these constants.
 */

#define FIXNUM_TAG_BITS       1
#define FIXNUM_TAG_MASK    0x01
#define FIXNUM_TAG         0x01

#define SHORT_TAG_BITS        3
#define SHORT_TAG_MASK     0x07
#define HEAP_TAG           0x00
#define FORWARD_TAG        0x02
#define IMMEDIATE_TAG      0x06

#define LONG_TAG_BITS         8
#define LONG_TAG_MASK      0xFF
#define CHARACTER_TAG      0x06
#define BOOLEAN_TAG        0x0E
#define READ_ACTION_TAG    0x16
#define SPECIAL_TAG        0x1E

#define FIXNUM_SHIFT          1
#define CHARACTER_SHIFT       8
#define BOOLEAN_SHIFT         8
#define READ_ACTION_SHIFT     8
#define SPECIAL_SHIFT         8

#define FIXNUM_OBJ(n)      ((obj_t)((n) << FIXNUM_SHIFT      | FIXNUM_TAG))
#define CHARACTER_OBJ(n)   ((obj_t)((n) << CHARACTER_SHIFT   | CHARACTER_TAG))
#define BOOLEAN_OBJ(n)     ((obj_t)((n) << BOOLEAN_SHIFT     | BOOLEAN_TAG))
#define READ_ACTION_OBJ(n) ((obj_t)((n) << READ_ACTION_SHIFT | READ_ACTION_TAG))
#define SPECIAL_OBJ(n)     ((obj_t)((n) << SPECIAL_SHIFT     | SPECIAL_TAG))

#define EMPTY_LIST         (SPECIAL_OBJ(0))
#define UNDEFINED_OBJ      (SPECIAL_OBJ(1))
#define END_OF_FILE        (SPECIAL_OBJ(2))
#define MISSING_ARG        (SPECIAL_OBJ(3))
#define MEM_OPS_PRIMITIVE  (SPECIAL_OBJ(4))
#define END_OF_ARGS        (SPECIAL_OBJ(5))

#define OBJ_TYPE_PREDICATE(type)					\
    static inline bool is_##type(obj_t obj)				\
    {									\
	extern mem_ops_t type##_ops;					\
	return is_heap(obj) && *(mem_ops_t **)obj == &type##_ops;	\
    }

/*
 * The unit of heap allocation is the word_t.  A word_t is an integer
 * the same size as a pointer.  uword_t is its unsigned twin.
 *
 * A Scheme object is an obj_t.  An obj_t is a pointer to an opaque
 * type.
 */

typedef struct object *obj_t;
typedef intptr_t       word_t;
typedef uintptr_t      uword_t;
typedef struct mem_ops mem_ops_t;

#define WORD_MIN  (INTPTR_MIN)
#define WORD_MAX  (INTPTR_MAX)
#define UWORD_MAX (UINTPTR_MAX)

static inline word_t obj_bits(const obj_t o)
{
    return (word_t)o;
}

static inline bool is_heap(const obj_t o)
{
    return (obj_bits(o) & SHORT_TAG_MASK) == HEAP_TAG;
}

extern const wchar_t *obj_type_name(const obj_t);

#endif /* !OBJ_INCLUDED */
