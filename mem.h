#ifndef MEM_INCLUDED
#define MEM_INCLUDED

#include <stddef.h>
#include <stdint.h>

#include "bool.h"

/*
 * Low-level memory manager definitions.
 */

/*
 * The unit of heap allocation is the word_t.  A word_t is an integer
 * the same size as a pointer.
 */

/*
 * There are four kinds of object pointers in the heap.  They can be
 * distinguished by their low-order three bits.
 *
 * - "Normal" pointers point to heap objects.  All heap objects are
 *      8-byte aligned, so the bottom three bits of a normal pointer
 *      are 000.
 *
 * - "Fixnum" pointers don't point to memory.  Instead, the value is
 *      stored in the pointer left-shifted one place.  The bottom bit
 *      of a fixnum is 1.  On an architecture with 32 bit pointers,
 *      fixnums range from -(2**31) to +(2**31 - 1).  With 64 bit
 *      pointers, they range from -(2**63) to +(2**63 - 1).
 *
 * - "Immediate" pointers don't point to memory.  Instead, the value
 *      is stored as a special bit pattern in the pointer itself.
 *      Immediate objects include Booleans, characters, and various
 *      special constants.  The bottom three bits of an immediate
 *      pointer are 110.
 *
 *      Non-fixnum immediate value are further subdivided thus.
 *
 *         0000 0110 - character.  The character value is stored in
 *                 bits 8-31 if Unicode, or bits 8-15 if ASCII.
 *
 *         0000 1110 - Boolean.
 *              0 0000 1010 - false
 *              1 0000 1010 - true
 *
 *         0001 0110 - special constant
 *             00 0001 0110 - nil (the empty list)
 *             01 0001 0110 - undefined
 *             10 0001 0110 - end of file
 *             11 0001 0110 - mem ops primitive (see below)
 *
 * - "Forwarding" pointers are used by the garbage collector, and
 *      are never seen by the mutator.  While a heap region is being
 *      scavenged, an object that has already been copied to a new
 *      region has its first word overwritten by a forwarding pointer,
 *      which points to the object's new location.  The forwarding pointer
 *      points two bytes past the object's new location, so it ends
 *      in 010.
 *
 * To summarize:
 *              1 - fixnum
 *            000 - normal object pointer
 *            010 - forwarding pointer
 *            100 - (reserved)
 *            110 - immediate non-fixnum
 *         0 0110 - character
 *         0 1110 - Boolean
 *    0 0000 1110 - false
 *    1 0000 1110 - true
 *         1 0110 - special constant
 *   00 0001 0110 - nil
 *   01 0001 0110 - undefined
 *   10 0001 0110 - end of file
 *   11 0001 0110 - memory ops primitive
 *         1 1100 - (reserved)
 */

typedef struct object     *obj_t;
typedef struct obj_header  obj_header_t;
typedef intptr_t           word_t;

#define OBJ_ALIGN             8

#define FIXNUM_TAG_MASK    0x01
#define FIXNUM_TAG_BITS       1
#define FIXNUM_TAG         0x01

#define SHORT_TAG_MASK     0x07
#define SHORT_TAG_BITS        3
#define NORMAL_TAG         0x00
#define FORWARD_TAG        0x02
#define IMMEDIATE_TAG      0x06

#define LONG_TAG_MASK      0x1F
#define LONG_TAG_BITS         5
#define CHARACTER_TAG      0x06
#define BOOLEAN_TAG        0x0E
#define SPECIAL_TAG        0x16

#define CHARACTER_SHIFT       8
#define BOOLEAN_SHIFT         8

#define FALSE_OBJ          ((obj_t)0x00E)
#define TRUE_OBJ           ((obj_t)0x10E)

#define EMPTY_LIST         ((obj_t)0x016)
#define UNDEFINED          ((obj_t)0x116)
#define END_OF_FILE        ((obj_t)0x216)
#define MEM_OPS_PRIMITIVE          0x316

static inline word_t obj_bits(const obj_t o)
{
    return (word_t)o;
}

static inline bool is_fixnum(const obj_t o)
{
    return (obj_bits(o) & FIXNUM_TAG_MASK) == FIXNUM_TAG;
}

static inline bool is_normal(const obj_t o)
{
    return (obj_bits(o) & SHORT_TAG_MASK) == NORMAL_TAG;
}

static inline bool is_forward(const obj_t o)
{
    return (obj_bits(o) & SHORT_TAG_MASK) == FORWARD_TAG;
}

static inline bool is_immediate(const obj_t o)
{
    return (obj_bits(o) & SHORT_TAG_MASK) == IMMEDIATE_TAG;
}

static inline bool is_character(const obj_t o)
{
    return (obj_bits(o) & LONG_TAG_MASK) == CHARACTER_TAG;
}

static inline bool is_boolean(const obj_t o)
{
    return (obj_bits(o) & LONG_TAG_MASK) == BOOLEAN_TAG;
}

static inline bool is_special(const obj_t o)
{
    return (obj_bits(o) & LONG_TAG_MASK) == SPECIAL_TAG;
}

static inline word_t obj_fixnum_value(const obj_t p)
{
    return obj_bits(p) >> FIXNUM_TAG_BITS;
}

static inline obj_header_t *obj_normal_ptr(obj_t o)
{
    return (obj_header_t *)o;
}

static inline obj_header_t *obj_fwd_ptr(obj_t o)
{
    return (obj_header_t *)(obj_bits(o) & ~SHORT_TAG_MASK);
}

static inline void header_set_fwd_ptr(obj_header_t *dst, obj_t fwd)
{
    *(word_t *)dst = obj_bits(fwd) | FORWARD_TAG;
}

/*
 * Normal objects can be further subdivided into primitives and record
 * instances.
 *
 * Each primitive object has a pointer to a mem_ops_t structure as its
 * first word.  Each primitive type has a mem_ops_t structure which is
 * stored outside the heap, and all instances of that type point to
 * the same structure.  The first field in a primitive's mem_ops_t is
 * MEM_OPS_PRIMITIVE, a special constant that appears nowhere inside
 * the heap.  Primitive types include string, vector, and record type
 * descriptor (rtd).
 *
 * Record instances are as described in r6rs-lib.  They support single
 * inheritance, field accessors, and several other properties.  The
 * first word of each record instance points to a rtd.
 * 
 * Record type descriptors (rtds) are primitive object allocated
 * in the heap.  Their layout starts with a mem_ops_t structure,
 * except that the first word of the rtd is a mem_ops_t pointer
 * instead of MEM_OPS_PRIMITIVE.
 *
 * Note that an rtd also has a superclass pointer which is distinct
 * from the mem_ops_t pointer.
 */

/* return the object's size in bytes. */
typedef size_t mem_size_op(const obj_header_t *);

/* return the number of object pointers in the object. */
typedef size_t mem_ptr_count_op(const obj_header_t *);

/* move the object to the new location. */
typedef void   mem_move_op(const obj_header_t *src, obj_header_t *dst);

/* return the object's i'th heap pointer. */
typedef obj_t  mem_get_ptr_op(const obj_header_t *, size_t index);

/* set the object's i'th heap pointer. */
typedef void   mem_set_ptr_op(obj_header_t *, size_t index, obj_t);

typedef struct mem_end_marker { } mem_end_marker_t;

typedef struct mem_ops mem_ops_t;
struct mem_ops {
    word_t                mo_start_marker;
    const wchar_t        *mo_name;	/* object class's name */
    mem_ops_t            *mo_super;	/* superclass pointer */
    mem_size_op          *mo_size;
    mem_ptr_count_op     *mo_ptr_count;
    mem_move_op          *mo_move;
    mem_get_ptr_op       *mo_get_ptr;
    mem_set_ptr_op       *mo_set_ptr;
    mem_end_marker_t      mo_end_marker;
};

struct obj_header {
    mem_ops_t *oh_ops;
};

static inline obj_header_t *obj_header(obj_t obj)
{
    return (obj_header_t *) obj;
}

static inline mem_ops_t *header_mem_ops(obj_header_t *header)
{
    return header->oh_ops;
}

static inline mem_ops_t *obj_mem_ops(obj_t obj)
{
    return header_mem_ops(obj_header(obj));
}

static inline bool is_primitive_obj(obj_header_t *header)
{
    return header->oh_ops->mo_start_marker == MEM_OPS_PRIMITIVE;
}

static inline bool is_record_instance(obj_header_t *header)
{
    return !is_primitive_obj(header);
}

/* set_heap_size_bytes must be called  before init_heap. */
extern void set_heap_size_bytes(size_t usable_size_bytes);

/* init_heap must be called before mem_alloc_obj. */
extern void init_heap(void);

#ifdef NDEBUG
    #define check_obj(obj) ((void)0)
#else
    extern void check_obj(const obj_t);
#endif

extern obj_header_t *mem_alloc_obj(mem_ops_t *, size_t size_bytes);

#endif /* !MEM_INCLUDED */
