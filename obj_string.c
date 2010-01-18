#include "obj_string.h"

#include <string.h>

#include "except.h"
#include "mem_scalar.h"
#include "obj_fixnum.h"

typedef struct string_obj {
    heap_object_t string_header;
    size_t        string_len;
    char_t        string_value[1];	/* NUL-terminated */
} string_obj_t;

static mem_ops_t string_ops;

static inline size_t len_to_bytes(size_t len)
{
    return sizeof (string_obj_t) + len * sizeof (char_t);
}

static size_t string_size_op(const heap_object_t *hobj)
{
    return len_to_bytes(((string_obj_t *)hobj)->string_len);
}

obj_t make_string_uninitialized(size_t len)
{
    if (!string_ops.mo_super)
	mem_scalar_create_ops(&string_ops, L"string", string_size_op);
    heap_object_t *hobj = mem_alloc_obj(&string_ops, len_to_bytes(len));
    string_obj_t *sp = (string_obj_t *)hobj;
    sp->string_len = len;
    sp->string_value[len] = L'\0';
    return (obj_t)hobj;
}

obj_t make_string_fill(size_t len, char_t fill)
{
    obj_t obj = make_string_uninitialized(len);
    string_obj_t *sp = (string_obj_t *)obj;
    int i;
    for (i = 0; i < len; i++)
	sp->string_value[i] = fill;
    return obj;
}

obj_t make_string_from_chars(const char_t *value, size_t len)
{
    obj_t obj = make_string_uninitialized(len);
    string_obj_t *sp = (string_obj_t *)obj;
    memcpy(sp->string_value, value, len * sizeof *value);
    return obj;
}

bool is_string(obj_t obj)
{
    check_obj(obj);
    return is_normal(obj) && obj_mem_ops(obj) == &string_ops;
}

size_t string_len(obj_t string)
{
    check_obj(string);
    CHECK(is_string(string), string, "string-length: must be string");
    return ((string_obj_t *)string)->string_len;
}

const char_t *string_value(obj_t string)
{
    check_obj(string);
    CHECK(is_string(string), string, "string-value: must be string");
    return ((string_obj_t *)string)->string_value;
}

void string_set_char(obj_t string, size_t index, char_t c)
{
    check_obj(string);
    CHECK(is_string(string), string, "string-set! must be string");
    string_obj_t *sp = (string_obj_t *)string;
    CHECK(index < sp->string_len,
	  string, "string-set! index out of range");
    CHECK(index < sp->string_len,
	  string, "string-set! index out of range", make_fixnum(index));
    sp->string_value[index] = c;
    ASSERT(sp->string_value[sp->string_len] == '\0');
}

void string_set_substring(obj_t         string,
			  size_t        pos,
			  size_t        len,
			  const char_t *substring)
{
    check_obj(string);
    CHECK(is_string(string), string, "string-set-substring! must be string");
    string_obj_t *sp = (string_obj_t *)string;
    CHECK(pos + len <= sp->string_len,
	  string, "string_set_substring! substring out of range",
	  make_fixnum(pos), make_fixnum(len));
    size_t i;
    char_t *p = sp->string_value;
    for (i = 0; i < len; i++)
	p[pos + i] = substring[i];
    ASSERT(sp->string_value[sp->string_len] == '\0');
}

int strings_cmp(obj_t str1, obj_t str2)
{
    CHECK(is_string(str1), str1, "string-cmp: must be string");
    CHECK(is_string(str2), str2, "string-cmp: must be string");
    size_t len1 = string_len(str1);
    size_t len2 = string_len(str2);
    const char_t *val1 = string_value(str1);
    const char_t *val2 = string_value(str2);
    size_t i;
    for (i = 0; i < len1 && i < len2; i++)
	if (val1[i] != val2[i])
	    return val1[i] - val2[i];
    return len1 - len2;
}
