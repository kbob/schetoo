#ifndef NORETURN_INCLUDED
#define NORETURN_INCLUDED

#ifdef __GNUC__
    #define NORETURN __attribute__ ((__noreturn__))
#else
    #define NORETURN
#endif

#endif /* !NORETURN_INCLUDED */
