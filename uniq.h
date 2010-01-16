#ifndef UNIQ_INCLUDED
#define UNIQ_INCLUDED

/* generate a unique identifier */
#define UNIQ_IDENT(prefix) CAT_(prefix, __LINE__)

/* concatenate into one identifier */
#define CAT_(a, b) CAT__(a, b)
#define CAT__(a, b) a ## b

#endif /* !UNIQ_INCLUDED */
