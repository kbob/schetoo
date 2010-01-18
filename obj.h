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

#define FIXNUM_TAG_MASK    0x01
#define FIXNUM_TAG_BITS       1
#define FIXNUM_TAG         0x01

#define SHORT_TAG_MASK     0x07
#define SHORT_TAG_BITS        3
#define HEAP_TAG           0x00
#define FORWARD_TAG        0x02
#define IMMEDIATE_TAG      0x06

#define LONG_TAG_MASK      0x1F
#define LONG_TAG_BITS         5
#define CHARACTER_TAG      0x06
#define BOOLEAN_TAG        0x0E
#define SPECIAL_TAG        0x16

#define FIXNUM_SHIFT          1
#define CHARACTER_SHIFT       8
#define BOOLEAN_SHIFT         8

#define FALSE_OBJ          ((obj_t)0x00E)
#define TRUE_OBJ           ((obj_t)0x10E)

#define EMPTY_LIST         ((obj_t)0x016)
#define UNDEFINED          ((obj_t)0x116)
#define END_OF_FILE        ((obj_t)0x216)
#define MEM_OPS_PRIMITIVE          0x316

#define OBJ_TYPE_PREDICATE(type)					\
    static inline bool is_##type(obj_t obj)				\
    {									\
	extern mem_ops_t type##_ops;					\
	return is_heap(obj) && *(mem_ops_t **)obj == &type##_ops;	\
    }

#define READER_CONSTANT(n) ((n) << 8 | 0x816)

/*
 * The unit of heap allocation is the word_t.  A word_t is an integer
 * the same size as a pointer.
 *
 * A Scheme object is an obj_t.  An obj_t is a pointer to an opaque
 * type.
 */

typedef struct object *obj_t;
typedef intptr_t       word_t;
typedef struct mem_ops mem_ops_t;

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

