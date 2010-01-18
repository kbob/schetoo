#ifndef CHAR_INCLUDED
#define CHAR_INCLUDED

#include <stdint.h>

#if SCHEME_FEATURE_UNICODE
    typedef int_least32_t char_t;
    typedef int_least32_t charint_t;
#else
    typedef char char_t;
    typedef int charint_t;
#endif

#endif /* !CHAR_INCLUDED */
