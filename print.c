#include "print.h"

#include <wctype.h>

#include "types.h"
#include "unicode.h"

/* XXX Reimplement this in Scheme. */

static inline bool is_ident_initial(wchar_t wc)
{
    /* Ruthless elision of braces is fun! */
    if (wc < 128)
	return iswalpha(wc) || wcschr(L"!$%&*/:<=>?^_~.+-", wc);
    else
	switch ((int)unicode_general_category(wc))
	case UGC_OTHER_PRIVATE_USE:
	case UGC_LETTER_LOWERCASE:
	case UGC_LETTER_MODIFIER:
	case UGC_LETTER_OTHER:
	case UGC_LETTER_TITLECASE:
	case UGC_LETTER_UPPERCASE:
	case UGC_MARK_NONSPACING:
	case UGC_NUMBER_LETTER:
	case UGC_NUMBER_OTHER:
	case UGC_PUNCTUATION_CONNECTOR:
	case UGC_PUNCTUATION_DASH:
	case UGC_PUNCTUATION_OTHER:
	case UGC_SYMBOL_CURRENCY:
	case UGC_SYMBOL_MODIFIER:
	case UGC_SYMBOL_MATH:
	case UGC_SYMBOL_OTHER:
	    return true;
    return false;
}

/* XXX this function duplicates one in scan.c. */
static inline bool is_ident_subsequent(wchar_t wc)
{
    if (is_ident_initial(wc) || wcschr(L"+-.@", wc))
	return true;
    switch ((int)unicode_general_category(wc)) {
    case UGC_NUMBER_DECIMAL_DIGIT:
    case UGC_MARK_SPACING_COMBINING:
    case UGC_MARK_ENCLOSING:
	return true;
    }
    return false;
}

static void print_form(obj_t, outstream_t *);

static void print_list_interior(obj_t obj, wchar_t *sep, outstream_t *out)
{
    while (!is_null(obj)) {
	outstream_printf(out, sep);
	print_form(pair_car(obj), out);
	obj = pair_cdr(obj);
	if (!is_null(obj) && !is_pair(obj)) {
	    outstream_printf(out, L" . ");
	    print_form(obj, out);
	    break;
	}
	sep = L" ";
    }
}

static void print_pair(obj_t obj, outstream_t *out)
{
    outstream_putwc(L'(', out);
    print_list_interior(obj, L"", out);
    outstream_putwc(L')', out);
}

static inline bool is_printing(wchar_t wc)
{
    if (wc < 128)
	return wc > L' ' && wc < L'\177';
    else
	switch ((int)unicode_general_category(wc))
	case UGC_OTHER_PRIVATE_USE:
	case UGC_LETTER_LOWERCASE:
	case UGC_LETTER_MODIFIER:
	case UGC_LETTER_OTHER:
	case UGC_LETTER_TITLECASE:
	case UGC_LETTER_UPPERCASE:
	case UGC_MARK_SPACING_COMBINING:
	case UGC_MARK_ENCLOSING:
	case UGC_MARK_NONSPACING:
	case UGC_NUMBER_DECIMAL_DIGIT:
	case UGC_NUMBER_LETTER:
	case UGC_NUMBER_OTHER:
	case UGC_PUNCTUATION_CONNECTOR:
	case UGC_PUNCTUATION_DASH:
	case UGC_PUNCTUATION_OTHER:
	case UGC_SYMBOL_CURRENCY:
	case UGC_SYMBOL_MODIFIER:
	case UGC_SYMBOL_MATH:
	case UGC_SYMBOL_OTHER:
	    return true;
    return false;
}

static void print_char(obj_t chobj, outstream_t *out)
{
    wchar_t wc = character_value(chobj);

    if (wc == L' ')
	outstream_printf(out, L"#\\space");
    else if (is_printing(wc))
	outstream_printf(out, L"#\\%lc", wc);
    else
	outstream_printf(out, L"#\\x%04x", wc);
}

static void print_string(obj_t string, outstream_t *out)
{
    size_t i, len = string_len(string);
    const wchar_t *value = string_value(string);
    outstream_putwc(L'"', out);
    for (i = 0; i < len; i++)
	outstream_putwc(value[i], out);
    outstream_putwc(L'"', out);
}

static void print_symbol(obj_t obj, outstream_t *out)
{
    obj_t string = symbol_name(obj);
    const wchar_t *value = string_value(string);
    size_t i, len = string_len(string);
    for (i = 0; i < len; i++) {
	wchar_t wc = value[i];
	if ((i ? is_ident_subsequent :  is_ident_initial)(wc))
	    outstream_putwc(value[i], out);
	else
	    outstream_printf(out, L"\\x%x;", wc);
    }
}

static void print_procedure(obj_t obj, outstream_t *out)
{
    //    if (procedure_is_special_form(obj) || procedure_is_C(obj)) {
    if (procedure_is_C(obj)) {
	outstream_printf(out, L"#<proc-%s%s%s>",
			 procedure_args_evaluated(obj) ? "" : "S",
			 procedure_is_C(obj) ? "C" : "",
			 procedure_is_raw(obj) ? "R" : "");
    } else {
	outstream_printf(out, L"(lambda ");
	print_form(procedure_args(obj), out);
	print_list_interior(procedure_body(obj), L" ", out);
	outstream_printf(out, L")");
    }
}

static void print_binding(obj_t obj, outstream_t *out)
{
    print_form(binding_name(obj), out);
    outstream_putwc(binding_is_mutable(obj) ? L':' : L'!', out);
    print_form(binding_value(obj), out);
}

static void print_vector(obj_t obj, outstream_t *out)
{
    size_t i, size = vector_len(obj);
    const wchar_t *sep = L"";
    outstream_printf(out, L"#(");
    for (i = 0; i < size; i++) {
	outstream_printf(out, sep);
	print_form(vector_ref(obj, i), out);
	sep = L" ";
    }    
    outstream_printf(out, L")");
}

static void print_bytevector(obj_t obj, outstream_t *out)
{
    size_t i, size = bytevector_len(obj);
    const wchar_t *sep = L"";
    outstream_printf(out, L"#vu8(");
    for (i = 0; i < size; i++) {
	outstream_printf(out, sep);
	outstream_printf(out, L"%d", bytevector_get(obj, i));
	sep = L" ";
    }    
    outstream_printf(out, L")");
}

static void print_form(obj_t obj, outstream_t *out)
{
    if (is_null(obj) || is_pair(obj)) {
	print_pair(obj, out);
    } else if (is_boolean(obj)) {
	outstream_printf(out, boolean_value(obj) ? L"#t" : L"#f");
    } else if (is_fixnum(obj)) {
	outstream_printf(out, L"%lld", (long long)fixnum_value(obj));
    } else if (is_character(obj)) {
	print_char(obj, out);
    } else if (is_string(obj)) {
	print_string(obj, out);
    } else if (is_symbol(obj)) {
	print_symbol(obj, out);
    } else if (is_procedure(obj)) {
        print_procedure(obj, out);
    } else if (is_binding(obj)) {
        print_binding(obj, out);
    } else if (is_vector(obj)) {
        print_vector(obj, out);
    } else if (is_bytevector(obj)) {
	print_bytevector(obj, out);
    } else {
	outstream_printf(out, L"#<%ls-%p>", obj_type_name(obj), obj);
    }
}

void princ(obj_t obj, outstream_t *out)
{
     print_form(obj, out);
}

void print(obj_t obj, outstream_t *out)
{
    if (!is_undefined(obj)) {
	print_form(obj, out);
	outstream_putwc(L'\n', out);
    }
}
