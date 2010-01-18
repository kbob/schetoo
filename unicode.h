#ifndef UNICODE_INCLUDED
#define UNICODE_INCLUDED

#include <stddef.h>

typedef enum unicode_general_category_t {

    UGC_LETTER_UPPERCASE,		/* Lu */
    UGC_LETTER_LOWERCASE,		/* Ll */
    UGC_LETTER_TITLECASE,		/* Lt */
    UGC_LETTER_MODIFIER,		/* Lm */
    UGC_LETTER_OTHER,			/* Lo */

    UGC_MARK_NONSPACING,		/* Mn */
    UGC_MARK_SPACING_COMBINING,		/* Mc */
    UGC_MARK_ENCLOSING,			/* Me */

    UGC_NUMBER_DECIMAL_DIGIT,		/* Nd */
    UGC_NUMBER_LETTER,			/* Nl */
    UGC_NUMBER_OTHER,			/* No */

    UGC_PUNCTUATION_CONNECTOR,		/* Pc */
    UGC_PUNCTUATION_DASH,		/* Pd */
    UGC_PUNCTUATION_OPEN,		/* Ps */
    UGC_PUNCTUATION_CLOSE,		/* Pe */
    UGC_PUNCTUATION_INITIAL_QUOTE,	/* Pi */
    UGC_PUNCTUATION_FINAL_QUOTE,	/* Pf */
    UGC_PUNCTUATION_OTHER,		/* Po */

    UGC_SYMBOL_MATH,			/* Sm */
    UGC_SYMBOL_CURRENCY,		/* Sc */
    UGC_SYMBOL_MODIFIER,		/* Sk */
    UGC_SYMBOL_OTHER,			/* So */

    UGC_SEPARATOR_SPACE,		/* Zs */
    UGC_SEPARATOR_LINE,			/* Zl */
    UGC_SEPARATOR_PARAGRAPH,		/* Zp */

    UGC_OTHER_CONTROL,			/* Cc */
    UGC_OTHER_FORMAT,			/* Cf */
    UGC_OTHER_SURROGATE,		/* Cs */
    UGC_OTHER_PRIVATE_USE,		/* Co */
    UGC_OTHER_NOT_ASSIGNED,		/* Cn */

} unicode_general_category_t;

unicode_general_category_t unicode_general_category(wchar_t);

const wchar_t *unicode_gc_short_name(unicode_general_category_t);

#endif /* !UNICODE_INCLUDED */
