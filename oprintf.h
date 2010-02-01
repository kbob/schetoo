#ifndef OPRINTF_INCLUDED
#define OPRINTF_INCLUDED

/*
 * oprintf - object-aware printf
 *
 * This version of printf() recognizes the %O conversion to print
 * an arbitrary Scheme object.
 */

extern int oprintf(const char *format, ...);

#endif /* !OPRINTF_INCLUDED */
