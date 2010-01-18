#ifndef CHARBUF_INCLUDED
#define CHARBUF_INCLUDED

#include "obj.h"

typedef struct charbuf {
    obj_t  cb_buf;
    size_t cb_pos;
    size_t cb_len;
} charbuf_t;

extern void           init_charbuf       (charbuf_t *, const wchar_t *prefix);
extern void           charbuf_append_char(charbuf_t *, wchar_t);
extern size_t         charbuf_len        (const charbuf_t *);
extern const wchar_t *charbuf_C_str      (const charbuf_t *);
extern obj_t          charbuf_make_string(charbuf_t *);

#endif /* !OBJ_STRING_INCLUDED */
