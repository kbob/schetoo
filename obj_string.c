#include "obj_string.h"

#include <assert.h>
#include <string.h>
#include <wchar.h>

#include "except.h"
#include "mem_scalar.h"
#include "obj_fixnum.h"

typedef struct string_obj {
    heap_object_t string_header;
    size_t        string_len;
    char_t        string_value[1];	/* NUL-terminated */
} string_obj_t;

mem_ops_t string_ops;

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

obj_t make_string_from_C_str(const char_t *value)
{
    return make_string_from_chars(value, wcslen(value));
}

size_t string_len(obj_t string)
{
    CHECK_OBJ(string);
    CHECK(is_string(string), "must be string", string);
    return ((string_obj_t *)string)->string_len;
}

const char_t *string_value(obj_t string)
{
    CHECK_OBJ(string);
    CHECK(is_string(string), "must be string", string);
    return ((string_obj_t *)string)->string_value;
}

const char_t *string_value_nc(obj_t string)
{
    CHECK_OBJ(string);
    assert(is_string(string));
    return ((string_obj_t *)string)->string_value;
}

void string_set_char_nc(obj_t string, size_t index, char_t c)
{
    CHECK_OBJ(string);
    assert(is_string(string));
    string_obj_t *sp = (string_obj_t *)string;
    assert(index < sp->string_len);
    MUTATE(string);
    sp->string_value[index] = c;
    assert(sp->string_value[sp->string_len] == L'\0');
}

void string_set_char_nm(obj_t string, size_t index, char_t c)
{
    /*
     * Non-mutating version  -- caller is responsible for
     * ensuring transaction semantics are met.
     */

    CHECK_OBJ(string);
    CHECK(is_string(string), "must be string", string);
    string_obj_t *sp = (string_obj_t *)string;
    //XXX CHECK(is_mutable(string), "must be mutable", string);
    CHECK(index < sp->string_len, "index out of range",
	  string, make_fixnum(index));
    sp->string_value[index] = c;
    assert(sp->string_value[sp->string_len] == '\0');
}

void string_set_char(obj_t string, size_t index, char_t c)
{
    CHECK_OBJ(string);
    CHECK(is_string(string), "must be string", string);
    string_obj_t *sp = (string_obj_t *)string;
    //XXX CHECK(is_mutable(string), "must be mutable", string);
    CHECK(index < sp->string_len, "index out of range",
	  string, make_fixnum(index));
    MUTATE(string);
    sp->string_value[index] = c;
    assert(sp->string_value[sp->string_len] == '\0');
}

void string_set_substring(obj_t         string,
			  size_t        pos,
			  size_t        len,
			  const char_t *substring)
{
    CHECK_OBJ(string);
    CHECK(is_string(string), "must be string", string);
    string_obj_t *sp = (string_obj_t *)string;
    CHECK(pos + len <= sp->string_len, "substring out of range",
	  string, make_fixnum(pos), make_fixnum(len));
    size_t i;
    char_t *p = sp->string_value;
    MUTATE(string);
    for (i = 0; i < len; i++)
	p[pos + i] = substring[i];
    assert(sp->string_value[sp->string_len] == '\0');
}

void string_set_len(obj_t str, size_t len)
{
    assert(is_string(str));
    string_obj_t *sp = (string_obj_t *)str;
    assert(len <= sp->string_len);
    sp->string_len = len;
    sp->string_value[len] = L'\0';
    MUTATE(str);
}

int strings_cmp(obj_t str1, obj_t str2)
{
    CHECK(is_string(str1), "must be string", str1);
    CHECK(is_string(str2), "must be string", str2);
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
