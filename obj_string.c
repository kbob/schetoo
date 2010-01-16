#include "obj_string.h"

#include <string.h>
#include <wchar.h>

#include "mem_scalar.h"

typedef struct string_obj {
    obj_header_t string_header;
    size_t       string_len;
    wchar_t      string_value[1];	/* NUL-terminated */
} string_obj_t;

static mem_ops_t string_ops;

static inline size_t len_to_bytes(size_t len)
{
    return sizeof (string_obj_t) + len * sizeof (wchar_t);
}

static size_t string_size_op(const obj_t *obj)
{
    return len_to_bytes(((string_obj_t *)obj)->string_len);
}

obj_t *make_string(size_t len, wchar_t wc)
{
    if (!string_ops.mo_super)
	mem_scalar_create_ops(&string_ops, L"string", string_size_op);
    obj_t *obj = mem_alloc_obj(&string_ops, len_to_bytes(len));
    string_obj_t *sp = (string_obj_t *)obj;
    sp->string_len = len;
    int i;
    for (i = 0; i < len; i++)
	sp->string_value[i] = wc;
    sp->string_value[len] = L'\0';
    return obj;
}

obj_t *make_string_from_chars(const wchar_t *value, size_t len)
{
    obj_t *obj = make_string(len, L'\0');
    string_obj_t *sp = (string_obj_t *)obj;
    memcpy(sp->string_value, value, len * sizeof *value);
    return obj;
}

bool is_string(obj_t *obj)
{
    assert_in_tospace(obj);
    return !is_null(obj) && OBJ_MEM_OPS(obj) == &string_ops;
}

size_t string_len(obj_t *string)
{
    assert_in_tospace(string);
    assert(is_string(string));
    return ((string_obj_t *)string)->string_len;
}

const wchar_t *string_value(obj_t *string)
{
    assert_in_tospace(string);
    assert(is_string(string));
    return ((string_obj_t *)string)->string_value;
}

void string_set_char(obj_t *string, size_t index, wchar_t wc)
{
    assert_in_tospace(string);
    assert(is_string(string));
    string_obj_t *sp = (string_obj_t *)string;
    assert(index < sp->string_len);
    sp->string_value[index] = wc;
    assert(sp->string_value[sp->string_len] == '\0');
}

void string_set_substring(obj_t         *string,
			  size_t         pos,
			  size_t         len,
			  const wchar_t *substring)
{
    assert_in_tospace(string);
    assert(is_string(string));
    string_obj_t *sp = (string_obj_t *)string;
    assert(pos + len <= sp->string_len);
    size_t i;
    wchar_t *p = sp->string_value;
    for (i = 0; i < len; i++)
	p[pos + i] = substring[i];
    assert(sp->string_value[sp->string_len] == '\0');
}

int strings_cmp(obj_t *str1, obj_t *str2)
{
    assert(is_string(str1));
    assert(is_string(str2));
    size_t len1 = string_len(str1);
    size_t len2 = string_len(str2);
    const wchar_t *val1 = string_value(str1);
    const wchar_t *val2 = string_value(str2);
    size_t i;
    for (i = 0; i < len1 && i < len2; i++)
	if (val1[i] != val2[i])
	    return val1[i] - val2[i];
    return len1 - len2;
}
