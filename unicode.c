#include "unicode.h"

#include <assert.h>

#include "bool.h"

typedef struct unicode_data {
#if 0
    enum unicode_general_category_t  ud_general_category:5;
    bool                             ud_alphabetic:1;
    bool                             ud_is_numeric:1;
    bool                             ud_is_whitespace:1;
    bool                             ud_is_upper_case:1;
    bool                             ud_is_lower_case:1;
    bool                             ud_is_title_case:1;
    wchar_t                          ud_simple_upcase;
    wchar_t                          ud_simple_downcase;
    wchar_t                          ud_simple_titlecase;
    wchar_t                          ud_simple_foldcase;
    wchar_t                         *ud_full_upcase;
    wchar_t                         *ud_full_downcase;
    wchar_t                         *ud_full_titlecase;
    wchar_t                         *ud_full_foldcase;
#else
    unsigned char ud_general_category;
#endif
} unicode_data_t;

#include "ucd_data.h"

static inline const unicode_data_t *get_data(wchar_t c)
{
    size_t pn = c / UCD_PAGE_SIZE;
    size_t pi = c % UCD_PAGE_SIZE;
    size_t i = ucd_page_map[pn] * UCD_PAGE_SIZE + pi;
    return &ucd_data[i];
}

unicode_general_category_t unicode_general_category(wchar_t c)
{
    if (c < 0 || c >= UCD_CODEPOINT_COUNT)
	return UGC_OTHER_NOT_ASSIGNED;
    return get_data(c)->ud_general_category;
}

const wchar_t *unicode_gc_short_name(unicode_general_category_t cat)
{
    switch (cat) {
    case UGC_LETTER_UPPERCASE: 		return L"Lu";
    case UGC_LETTER_LOWERCASE: 		return L"Ll";
    case UGC_LETTER_TITLECASE: 		return L"Lt";
    case UGC_LETTER_MODIFIER: 		return L"Lm";
    case UGC_LETTER_OTHER: 		return L"Lo";

    case UGC_MARK_NONSPACING:        	return L"Mn";
    case UGC_MARK_SPACING_COMBINING: 	return L"Mc";
    case UGC_MARK_ENCLOSING:         	return L"Me";

    case UGC_NUMBER_DECIMAL_DIGIT: 	return L"Nd";
    case UGC_NUMBER_LETTER: 		return L"Nl";
    case UGC_NUMBER_OTHER: 		return L"No";

    case UGC_PUNCTUATION_CONNECTOR: 	return L"Pc";
    case UGC_PUNCTUATION_DASH: 		return L"Pd";
    case UGC_PUNCTUATION_OPEN:          return L"Ps";
    case UGC_PUNCTUATION_CLOSE:         return L"Pe";
    case UGC_PUNCTUATION_INITIAL_QUOTE: return L"Pi";
    case UGC_PUNCTUATION_FINAL_QUOTE: 	return L"Pf";
    case UGC_PUNCTUATION_OTHER: 	return L"Po";

    case UGC_SYMBOL_MATH: 		return L"Sm";
    case UGC_SYMBOL_CURRENCY: 		return L"Sc";
    case UGC_SYMBOL_MODIFIER: 		return L"Sk";
    case UGC_SYMBOL_OTHER: 		return L"So";

    case UGC_SEPARATOR_SPACE: 		return L"Zs";
    case UGC_SEPARATOR_LINE: 		return L"Zl";
    case UGC_SEPARATOR_PARAGRAPH: 	return L"Zp";

    case UGC_OTHER_CONTROL: 		return L"Cc";
    case UGC_OTHER_FORMAT: 		return L"Cf";
    case UGC_OTHER_SURROGATE: 		return L"Cs";
    case UGC_OTHER_PRIVATE_USE: 	return L"Co";
    case UGC_OTHER_NOT_ASSIGNED: 	return L"Cn";
    }
    assert(false);
}
