#include "arith.h"

#include <assert.h>			/* XXX */
#include <wchar.h>

#include "obj_boolean.h"
#include "obj_fixnum.h"

static inline int digit_value(wchar_t digit, int radix)
{
    static wchar_t digits[] = L"0123456789abcdef";
    const wchar_t *p = wcschr(digits, towlower(digit));
    if (p)
	return p - digits;
    else
	return -1;
}

obj_t chars_to_number(const char_t *str, size_t len, int radix)
{
    if (radix != 2 && radix != 010 && radix != 10 && radix != 0x10) {
	assert(0);
	return FALSE_OBJ;
    }
    size_t i = 0;
    int xc = 0, rc = 0;
    while (i < len - 1 && str[i] == L'#') {
	wchar_t wc = str[i + 1];
	i += 2;
	if (wc == 'E' || wc == 'e')
	    xc++;
	else if (wc == 'B' || wc == 'b') {
	    radix = 2;
	    rc++;
	}
	else if (wc == 'O' || wc == 'o') {
	    radix = 010;
	    rc++;
	}
	else if (wc == 'D' || wc == 'd') {
	    radix = 10;
	    rc++;
	}
	else if (wc == 'X' || wc == 'x') {
	    radix = 0x10;
	    rc++;
	}
	else {
	    assert(0);
	    return FALSE_OBJ;
	}
    }
    if (xc > 1 || rc > 1) {
	assert(0);
	return FALSE_OBJ;
    }
    int sign = +1;
    if (str[i] == L'+') {
	i++;
	sign = +1;
    }
    if (str[i] == L'-') {
	i++;
	sign = -1;
    }
    word_t n = 0;
    while (i < len) {
	word_t k = digit_value(str[i++], radix);
	if (k < 0) {
	    assert(0);
	    return FALSE_OBJ;
	}
	n = radix * n + k;
    }
    return make_fixnum(sign * n);
}
