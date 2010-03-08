#include <wchar.h>

#include <assert.h>

#include "prim.h"
#include "types.h"

DEFINE_PROC(L"integer?", 1)(obj_t obj)
{
    return make_boolean(is_fixnum(obj));
}

DEFINE_PROC(L"=", 2-)(obj_t z1, obj_t z2, obj_t zs)
{
    CHECK(is_fixnum(z1), "must be fixnum");
    while (true) {
	CHECK(is_fixnum(z2), "must be fixnum");
	if (!(fixnum_value(z1) == fixnum_value(z2)))
	    return FALSE_OBJ;
	if (is_null(zs))
	    return TRUE_OBJ;
	z1 = z2;
	z2 = CAR(zs);
	zs = CDR(zs);
    }
}

DEFINE_PROC(L"<", 2-)(obj_t z1, obj_t z2, obj_t zs)
{
    CHECK(is_fixnum(z1), "must be fixnum");
    while (true) {
	CHECK(is_fixnum(z2), "must be fixnum");
	if (!(fixnum_value(z1) < fixnum_value(z2)))
	    return FALSE_OBJ;
	if (is_null(zs))
	    return TRUE_OBJ;
	z1 = z2;
	z2 = CAR(zs);
	zs = CDR(zs);
    }
}

DEFINE_PROC(L">", 2-)(obj_t z1, obj_t z2, obj_t zs)
{
    CHECK(is_fixnum(z1), "must be fixnum");
    while (true) {
	CHECK(is_fixnum(z2), "must be fixnum");
	if (!(fixnum_value(z1) > fixnum_value(z2)))
	    return FALSE_OBJ;
	if (is_null(zs))
	    return TRUE_OBJ;
	z1 = z2;
	z2 = CAR(zs);
	zs = CDR(zs);
    }
}

DEFINE_PROC(L"+", 0-)(obj_t arg_list)
{
    word_t sum = 0;
    while (!is_null(arg_list)) {
	sum += fixnum_value(CAR(arg_list));
	arg_list = CDR(arg_list);
    }
    return make_fixnum(sum);
}

DEFINE_PROC(L"*", 0-)(obj_t arg_list)
{
    word_t product = 1;
    while (!is_null(arg_list)) {
	product *= fixnum_value(CAR(arg_list));
	arg_list = CDR(arg_list);
    }
    return make_fixnum(product);
}

DEFINE_PROC(L"-", 1-)(obj_t z, obj_t rest)
{
    if (is_null(rest))
	return make_fixnum(-fixnum_value(z));
    word_t diff = fixnum_value(z);
    while (!is_null(rest)) {
	diff -= fixnum_value(CAR(rest));
	rest = CDR(rest);
    }
    return make_fixnum(diff);
}

DEFINE_PROC(L"div", 2)(obj_t x1, obj_t x2)
{
    word_t v1 = fixnum_value(x1), v2 = fixnum_value(x2);
    CHECK(v2, "division by zero");
    word_t q = v1 / v2;
    word_t r = v1 - q * v2;
    if (r < 0) {
	if (v2 < 0)
	    q++;
	else
	    q--;
    }
    return make_fixnum(q);
}

DEFINE_PROC(L"mod", 2)(obj_t x1, obj_t x2)
{
    word_t v1 = fixnum_value(x1), v2 = fixnum_value(x2);
    CHECK(v2, "division by zero");
    word_t r = v1 % v2;
    if (r < 0) {
	if (v2 < 0)
	    r -= v2;
	else
	    r += v2;
    }
    return make_fixnum(r);
}

DEFINE_PROC(L"quotient", 2)(obj_t n1, obj_t n2)
{
    word_t v1 = fixnum_value(n1), v2 = fixnum_value(n2);
    CHECK(v2, "division by zero");
    return make_fixnum(v1 / v2);
}

DEFINE_PROC(L"remainder", 2)(obj_t n1, obj_t n2)
{
    word_t v1 = fixnum_value(n1), v2 = fixnum_value(n2);
    CHECK(v2, "division by zero");
    return make_fixnum(v1 % v2);
}

DEFINE_PROC(L"modulo", 2)(obj_t n1, obj_t n2)
{
    word_t v1 = fixnum_value(n1), v2 = fixnum_value(n2);
    CHECK(v2, "division by zero");
    word_t m = v1 % v2;
    if ((v2 < 0) != (m < 0))
	m += v2;
    return make_fixnum(m);
}

DEFINE_PROC(L"number->string", 1-3)(obj_t z, obj_t radix, obj_t precision)
{
    word_t zv = fixnum_value(z);
    int r;
    const char_t *fmt = L"%s%d";
    size_t max_chars = sizeof (word_t) * 8 + 2;
    wchar_t buf[max_chars];

    if (radix == MISSING_ARG)
	r = 10;
    else {
	r = fixnum_value(radix);
	switch (r) {
	case 2:
	    {
		size_t i = 0, j;
		if (zv < 0) {
		    buf[i++] = L'-';
		    zv = -zv;
		}
		for (j = 1; 1 << j < zv; j++)
		    continue;
		while (j--)
		    buf[i++] = "10"[!(zv & 1 << j)];
		return make_string_from_chars(buf, i);
	    }
	    break;
	case 010:
	    fmt = L"%s%o";
	    break;
	case 10:
	    break;
	case 0x10:
	    fmt = L"%s%x";
	    break;
	default:
	    CHECK(false, "illegal radix");
	}
	if (precision != MISSING_ARG)
	    THROW(&implementation_restriction, "inexact numbers unsupported");
    }
    word_t azv = zv < 0 ? -zv : zv;
    int nchar = swprintf(buf, max_chars, fmt, &"-"[zv >= 0], azv);
    assert(nchar > 0);
    return make_string_from_chars(buf, nchar);
}

static inline int digit_value(wchar_t digit, int radix)
{
    static wchar_t digits[] = L"0123456789abcdef";
    const wchar_t *p = wcschr(digits, towlower(digit));
    if (p)
	return p - digits;
    else
	return -1;
}

DEFINE_PROC(L"string->number", 1-2)(obj_t string, obj_t radix)
{
    const char_t *str = string_value(string);
    int r;
    if (radix == MISSING_ARG)
	r = 10;
    else {
	if (!is_fixnum(radix))
	    return FALSE_OBJ;
	r = fixnum_value(radix);
	if (r != 2 && r != 010 && r != 10 && r != 0x10)
	    return FALSE_OBJ;
    }
    size_t i = 0, len = string_len(string);
    int xc = 0, rc = 0;
    while (i < len - 1 && str[i] == L'#') {
	wchar_t wc = str[i + 1];
	i += 2;
	if (wc == 'E' || wc == 'e')
	    xc++;
	else if (wc == 'B' || wc == 'b') {
	    r = 2;
	    rc++;
	}
	else if (wc == 'O' || wc == 'o') {
	    r = 010;
	    rc++;
	}
	else if (wc == 'D' || wc == 'd') {
	    r = 10;
	    rc++;
	}
	else if (wc == 'X' || wc == 'x') {
	    r = 0x10;
	    rc++;
	}
	else
	    return FALSE_OBJ;
    }
    if (xc > 1 || rc > 1)
	return FALSE_OBJ;
    int sign = +1;
    if (str[i] == L'-') {
	i++;
	sign = -1;
    }
    word_t n = 0;
    while (i < len) {
	word_t k = digit_value(str[i++], r);
	if (k < 0)
	    return FALSE_OBJ;
	n = r * n + k;
    }
    return make_fixnum(sign * n);
}
