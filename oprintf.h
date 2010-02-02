#ifndef OPRINTF_INCLUDED
#define OPRINTF_INCLUDED

/*
 * oprintf - object-aware printf
 *
 * This version of printf() recognizes the %O conversion to print
 * an arbitrary Scheme object.
 */

/* (Actually, for GCC/glibc, we use register_printf_function() to add
 * %O support to printf().  oprintf()'s only advantage is that GCC
 * doesn't check its format, so it doesn't trigger a compiler warning
 * for an unknown conversion character.)
 */

extern int oprintf(const char *format, ...);

#endif /* !OPRINTF_INCLUDED */
