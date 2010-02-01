#ifndef RANGE_INCLUDED
#define RANGE_INCLUDED

#include "bool.h"

/*
 * Interval - too clever way of representing a numeric interval in a
 * C macro.  Any of these forms are valid intervals.
 *
 *     N is an interval of a single number.
 *     M-N is an interval from M to N, inclusive.
 *     M-  is an interval from M to infinity.
 *
 * We abuse the C preprocessor to change that into an integer constant
 * that can be unpacked by the following functions.
 *
 * M and N are restricted to the range 0..255.
 */

#define V_(ivl) (!!(0 * ivl + 0))
#define I_(ivl) (ivl + 1 < ivl + 0)
#define L_(ivl) ((ivl + 0) - (0 * ivl + 0))
#define U_(ivl) ((0 * ivl + 0) ?					\
                     -(0 * ivl + 0) :					\
		     (ivl + 0) - (0 * ivl + 0))

#define COMPILE_INTERVAL(ivl) \
    (V_(ivl) | I_(ivl) << 1 | L_(ivl) << 8 | U_(ivl) << 16)

typedef int interval_t;

static inline bool interval_is_variadic(interval_t ivl)
{
    // True if interval not a single number
    return ivl & 1;
}

static inline bool interval_is_infinite(interval_t ivl)
{
    // True if interval is unbounded
    return ivl & 2 >> 1;
}

static inline int interval_lower_bound(interval_t ivl)
{
    return ivl >> 8 & 0xFF;
}

static inline int interval_upper_bound(interval_t ivl)
{
    // Only defined if finite.
    return ivl >> 16 & 0xFF;
}

#endif /* !RANGE_INCLUDED */
