#include "charbuf.h"

#include <wchar.h>

#include "obj_string.h"

void init_charbuf_size(charbuf_t *cbp, size_t len)
{
    size_t pos = 0;
    cbp->cb_buf = make_string_uninitialized(len);
    cbp->cb_pos = pos;
    cbp->cb_len = len;
}
void init_charbuf(charbuf_t *cbp)
{
    init_charbuf_size(cbp, 16);
}

void charbuf_append_char(charbuf_t *cbp, wchar_t wc)
{
    if (cbp->cb_len == cbp->cb_pos) {
	cbp->cb_len *= 2;
	obj_t tmp = make_string_fill(cbp->cb_len, L'\0');
	string_set_substring(tmp, 0, cbp->cb_pos, string_value(cbp->cb_buf));
	cbp->cb_buf = tmp;
    }
    string_set_char_nc(cbp->cb_buf, cbp->cb_pos++, wc);
}

size_t charbuf_len(const charbuf_t *cbp)
{
    return cbp->cb_pos;
}

const wchar_t *charbuf_C_str(const charbuf_t *cbp)
{
    // Ensure string is NUL-terminated.
    charbuf_append_char((charbuf_t *)cbp, L'\0');
    ((charbuf_t *)cbp)->cb_pos--;
    return string_value(cbp->cb_buf);
}

obj_t charbuf_make_string(charbuf_t *cbp)
{
    string_set_len(cbp->cb_buf, cbp->cb_pos);
    return cbp->cb_buf;
}
