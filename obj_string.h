#ifndef OBJ_STRING_INCLUDED
#define OBJ_STRING_INCLUDED

#include "char.h"
#include "obj.h"

extern obj_t         make_string_uninitialized
                                         (size_t len);
extern obj_t         make_string_fill    (size_t len, char_t fill);
extern obj_t         make_string_from_chars
                                         (const char_t *value, size_t len);
extern bool          is_string           (obj_t);
extern size_t        string_len          (obj_t);
extern const char_t *string_value        (obj_t);
extern void          string_set_char     (obj_t, size_t index, char_t wc);
extern void          string_set_substring(obj_t         string,
					  size_t        pos,
					  size_t        len,
					  const char_t *substring);
extern int           strings_cmp         (obj_t str1, obj_t str2);

static inline bool   strings_are_equal   (obj_t str1, obj_t str2)
{
    return strings_cmp(str1, str2) == 0;
}

#endif /* !OBJ_STRING_INCLUDED */
