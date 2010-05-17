#include "scan.h"

#include "oprintf.h"			/* XXX */
#include "scan.h"
#include "dfa_data.h"

#include <assert.h>

#include "arith.h"
#include "charbuf.h"
#include "list.h"
#include "obj_cont.h"
#include "obj_eof.h"
#include "prim.h"
#include "record.h"
#include "test.h"
#include "transaction.h"
#include "types.h"
#include "unicode.h"

DEFINE_STATIC_RECORD_TYPE(scan_ctx, L"scan-context", NULL, 0) = {
    { FM_IMMUTABLE, L"state" },
    { FM_MUTABLE, L"buf" },
    { FM_MUTABLE, L"pos" },
    { FM_END },
};

static inline yy_state_t scan_ctx_state(obj_t ctx)
{
    return fixnum_value(record_get_field(ctx, 0));
}

static inline obj_t scan_ctx_buf(obj_t ctx)
{
    return record_get_field(ctx, 1);
}

static inline size_t scan_ctx_pos(obj_t ctx)
{
    return fixnum_value(record_get_field(ctx, 2));
}

static inline size_t scan_ctx_len(obj_t ctx)
{
    return string_len(scan_ctx_buf(ctx));
}

static inline obj_t make_scan_ctx(yy_state_t state, obj_t buf, size_t pos)
{
    return MAKE_RECORD(scan_ctx, make_fixnum(state), buf, make_fixnum(pos));
}

static inline obj_t make_new_scan_ctx(void)
{
    return make_scan_ctx(YY_INITIAL_STATE, make_string_fill(16, L'\0'), 0);
}

static obj_t scan_ctx_advance(obj_t ctx, yy_state_t state, char_t ch)
{
    obj_t  buf = scan_ctx_buf(ctx);
    size_t pos = scan_ctx_pos(ctx);
    size_t len = scan_ctx_len(ctx);
    assert(pos <= len);
    if (pos >= len) {
	obj_t old = buf;
	buf = make_string_fill(2 * pos, L'\0');
	string_set_substring(buf, 0, pos, string_value(old));
    }
    string_set_char_nm(buf, pos, ch);
    return make_scan_ctx(state, buf, pos + 1);
}

static inline bool is_digit(wchar_t wc)
{
    return L'0' <= wc && wc <= L'9';
}

static inline bool is_xdigit(wchar_t wc)
{
    if (is_digit(wc))
	return true;
    return (L'a' <= wc && wc <= L'f') || (L'A' <= wc && wc <= L'F');
}

static inline yy_cc_t char_class(wint_t wc)
{
    if (wc < 128)
	return yy_charmap[wc];
    else if (wc == L'\x0085')
	return YY_NEXT_LINE_CC;
    else if (wc == L'\x2028')
	return YY_LINE_SEPARATOR_CC;
    else if (wc == L'\x2029')
	return YY_PARAGRAPH_SEPARATOR_CC;
    else
      return yy_unicode_catmap[unicode_general_category(wc)];
   return -1;
}

static inline bool is_delimiter(obj_t ch)
{
    if (is_eof(ch))
	return true;
    wchar_t wc = character_value(ch);
    if (wc < 128) {
	switch (wc) {

	case L' ':
	case L'(':
	case L')':
	case L'[':
	case L']':
	case L'"':
	case L';':
	case L'#':
	case L'\t':
	case L'\n':
	case L'\v':
	case L'\f':
	case L'\r':
	    return true;

	default:
	    return false;
	}
    } else {
	if (wc == L'\x0085')
	    return true;
	switch (unicode_general_category(wc)) {

	case UGC_SEPARATOR_SPACE:
	case UGC_SEPARATOR_LINE:
	case UGC_SEPARATOR_PARAGRAPH:
	    return true;

	default:
	    return false;
	}
    }
}

static inline bool token_needs_delimiter(yy_token_t t)
{
    switch (t) {

    case YY_IDENTIFIER:
    case YY_PERIOD:
    case YY_NUMBER:
    case YY_CHARACTER:
    case YY_BOOLEAN:
	return true;

    default:
	return false;
    }
}

static inline bool is_intraline_whitespace(char_t ch)
{
    return ch == L'\t' || unicode_general_category(ch) == UGC_SEPARATOR_SPACE;
}

static inline int digit_value(char_t wc)
{
    if (L'0' <= wc && wc <= L'9')
	return wc - L'0';
    if (L'a'<= wc && wc <= L'f')
	return wc - L'a' + 0xa;
    assert(L'A'<= wc && wc <= L'F');
    return wc - L'A' + 0xA;
}

static word_t scan_hex_scalar(const char_t *str, char_t end)
{
    word_t xval = 0;
    bool empty = true;
    while (*str && is_xdigit(*str)) {
	xval = 0x10 * xval + digit_value(*str);
	empty = false;
	str++;
    }
    CHECK_CONDITION(!empty && *str == end,
		    &lexical, "malformed hex value",
		    make_string_from_C_str(str));
    CHECK_CONDITION(xval <= 0x10ffff && (xval < 0xd800 || xval > 0xdfff),
		    &lexical, "illegal character",
		    make_string_from_C_str(str));
    return xval;
}

static obj_t scan_identifier(const char_t *str, size_t length)
{
    int i, j;
    for (i = 0; i < length; i++)
	if (str[i] == L'\\') {
	    charbuf_t cbuf;
	    init_charbuf(&cbuf);
	    for (j = 0; j < length; ) {
		char_t ch = str[j];
		if (ch != L'\\') {
		    charbuf_append_char(&cbuf, ch);
		    j++;
		} else {
		    assert(j < length - 3);
		    assert(str[j + 1] == L'x');
		    charbuf_append_char(&cbuf,
					scan_hex_scalar(str + j + 2, L';'));
		    while (str[j++] != L';')
			assert(j < length);
		}		
	    }		
	    return make_symbol(charbuf_make_string(&cbuf));
	}

    /* No hex escapes, just make the symbol. */
    return make_symbol_from_chars(str, length);
}

static obj_t scan_boolean(const char_t *str, size_t length)
{
    assert(length == 2);
    switch (str[1]) {

    case 'T':
    case 't':
	return TRUE_OBJ;

    case 'F':
    case 'f':
	return FALSE_OBJ;

    default:
	assert(false);
    }
}

static obj_t scan_number(const char_t *str, size_t length)
{
    obj_t z = chars_to_number(str, length, 10);
    CHECK_CONDITION(z != FALSE_OBJ,
		    &implementation_restriction, "unsupported numeric literal",
		    make_string_from_chars(str, length));
    return z;
}

static obj_t scan_character(const char_t *str, size_t length)
{
    typedef struct char_name_map {
	wchar_t *cn_name;
	wchar_t  cn_char;
    } char_name_map_t;

    static const char_name_map_t char_names[] = {
	{ L"alarm",     L'\a'   },
	{ L"backspace", L'\b'   },
	{ L"delete",    L'\177' },
	{ L"esc",       L'\33'  },
	{ L"linefeed",  L'\n'   },
	{ L"newline",   L'\n'   },		/* deprecated */
	{ L"nul",       L'\0'   },
	{ L"page",      L'\f'   },
	{ L"return",    L'\r'   },
	{ L"space",     L' '    },
	{ L"tab",       L'\t'   },
	{ L"vtab",      L'\v'   },
    };

    static size_t char_name_count = sizeof char_names / sizeof *char_names;

    /* examples:  #\a  #\newline  #\x03b8;  */
    assert(str[0] == L'#' && str[1] == L'\\');
    assert(wcslen(str) == length);
    if (length == 3)
	return make_character(str[2]);
    assert(length > 3);
    if (str[2] == L'x') {
	word_t xval = scan_hex_scalar(str + 3, L'\0');
	CHECK_CONDITION(xval <= 0x10ffff && (xval < 0xd800 || xval > 0xdfff),
			&lexical, "illegal character",
			make_string_from_C_str(str));
	return make_character(xval);
    }
    const char_name_map_t *p;
    COULD_RETRY();
    for (p = char_names; p < char_names + char_name_count; p++)
	if (!wcscmp(str + 2, p->cn_name))
	    return make_character(p->cn_char);
    THROW(&lexical, "unknown character name",
	  make_string_from_C_str(str));
}

static obj_t scan_string(const char_t *str, size_t length)
{
    char_t ch;
    charbuf_t buf;
    init_charbuf(&buf);
    assert(str[0] == L'"');
    assert(str[length - 1] == L'"');
    size_t i;
    for (i = 1; i < length - 1; ) {
	if (str[i] != L'\\')
	    ch = str[i++];
	else {
	    switch (str[++i]) {

	    case 'a':
		ch = L'\a';
		break;

	    case 'b':
		ch = L'\b';
		break;

	    case 't':
		ch = L'\t';
		break;

	    case 'n':
		ch = L'\n';
		break;

	    case 'v':
		ch = L'\v';
		break;

	    case 'f':
		ch = L'\f';
		break;

	    case 'r':
		ch = L'\r';
		break;

	    case '"':
		ch = L'"';
		break;

	    case '\\':
		ch = L'\\';
		break;

	    case 'x':
	    case 'X':
		ch = scan_hex_scalar(str + i + 1, L';');
		while (str[i] != ';')
		    i++;
		break;

	    default:
		ch = str[i];
		while (is_intraline_whitespace(ch))
		    ch = str[++i];
		bool line_end = false;
		/* line_ending = \r \n \r\n \x85; \r\x85; \u2028; */
		while (wcschr(L"\r\n\x85\x2028", ch)) {
		    ch = str[++i];
		    line_end = true;
		}
		assert(line_end);
		while (is_intraline_whitespace(ch))
		    ch = str[++i];
		continue;
	    }
	    i++;
	}
	charbuf_append_char(&buf, ch);
    }
    return charbuf_make_string(&buf);
}

static token_type_t make_token(yy_state_t state, obj_t ctx, obj_t *yylval)
{
    EVAL_LOG("state=%d ctx=%O", state, ctx);
    obj_t  buf = record_get_field(ctx, 1);
    size_t len = fixnum_value(record_get_field(ctx, 2));
    assert(state < YY_ACCEPT_COUNT);
    const char_t *str = string_value(buf);
    token_type_t toktype;
    switch (yy_accepts[state]) {

    case YY_NUMBER:
	toktype = TOK_SIMPLE;
	*yylval = scan_number(str, len);
	if (is_fixnum(*yylval)) {
	    word_t n = fixnum_value(*yylval);
	    if (0 <= n && n <= 255)
		toktype = TOK_U8;
	}
	break;

    case YY_IDENTIFIER:
	toktype = TOK_SIMPLE;
	*yylval = scan_identifier(str, len);
	break;

    case YY_BOOLEAN:
	toktype = TOK_SIMPLE;
	*yylval = scan_boolean(str, len);
	break;

    case YY_CHARACTER:
	toktype = TOK_SIMPLE;
	*yylval = scan_character(str, len);
	break;
	
    case YY_STRING:
	toktype = TOK_SIMPLE;
	*yylval = scan_string(str, len);
	break;

    case YY_LPAREN:
	toktype = TOK_LPAREN;
	break;

    case YY_RPAREN:
	toktype = TOK_RPAREN;
	break;

    case YY_LBRACKET:
	toktype = TOK_LBRACKET;
	break;

    case YY_RBRACKET:
	toktype = TOK_RBRACKET;
	break;

    case YY_BEGIN_VECTOR:
	toktype = TOK_BEGIN_VECTOR;
	break;

    case YY_BEGIN_BYTEVECTOR:
	toktype = TOK_BEGIN_BYTEVECTOR;
	break;

    case YY_QUOTE:
	toktype = TOK_ABBREV;
	*yylval = make_symbol_from_C_str(L"quote");
	break;

    case YY_QUASIQUOTE:
	toktype = TOK_ABBREV;
	*yylval = make_symbol_from_C_str(L"quasiquote");
	break;

    case YY_UNQUOTE:
	toktype = TOK_ABBREV;
	*yylval = make_symbol_from_C_str(L"unquote");
	break;

    case YY_UNQUOTE_SPLICING:
	toktype = TOK_ABBREV;
	*yylval = make_symbol_from_C_str(L"unquote-splicing");
	break;

    case YY_PERIOD:
	toktype = TOK_PERIOD;
	break;

    case YY_SYNTAX:
	toktype = TOK_ABBREV;
	*yylval = make_symbol_from_C_str(L"syntax");
	break;

    case YY_QUASISYNTAX:
	toktype = TOK_ABBREV;
	*yylval = make_symbol_from_C_str(L"quasisyntax");
	break;

    case YY_UNSYNTAX:
	toktype = TOK_ABBREV;
	*yylval = make_symbol_from_C_str(L"unsyntax");
	break;

    case YY_UNSYNTAX_SPLICING:
	toktype = TOK_ABBREV;
	*yylval = make_symbol_from_C_str(L"unsyntax-splicing");
	break;

    case YY_BEGIN_DATUM_COMMENT:
	toktype = TOK_COMMENT;
	break;

    default:
	assert(false);
    }
    EVAL_LOG("returning toktype=%s *yylval=%O", token_name(toktype), *yylval);
    return toktype;
}

static cv_t c_continue_read_token(obj_t cont, obj_t value);

static cv_t c_discard(obj_t cont, obj_t values)
{
    EVAL_LOG("values=%O cont4_arg=%O", values, cont4_arg(cont));
    return cv(cont_cont(cont), cont4_arg(cont));
}

/*
 * Some tokens can only be accepted after the following character has
 * been seen.  The R6RS spec calls out some specific tokens with that
 * property, and token_needs_delimiter() identifies those.
 *
 * But we also need to check whether the DFA has any non-error transitions
 * away from the accepting state.  If that is the case, then we need to
 * see the next character and check whether it makes a good transition.
 */

static inline bool accept_early(yy_state_t state)
{
    /* Non-accepting states do not accept early. */
    if (state >= YY_ACCEPT_COUNT)
	return false;

    /* Delimiter-requiring tokens do not accept early. */
    yy_token_t tok = yy_accepts[state];
    if (token_needs_delimiter(tok))
	return false;

    /* If this state has non-error transitions, do not accept early. */
    const yy_delta_row_t *row = &yy_delta[state];
    assert(YY_COMMON_STATE == YY_ERROR_STATE);
    return row->yy_len == 0;
    /* (if COMMON_STATE != ERROR_STATE, need a more expensive check.) */

    /* Okay, accept early. */
    return true;
}

static inline bool accept_late(yy_state_t state, obj_t ch)
{
    /* Non-accepting states do not accept late. */
    if (state >= YY_ACCEPT_COUNT)
	return false;

    /* Delimiter-requiring tokens accept when the delimiter is seen. */
    yy_token_t tok = yy_accepts[state];
    if (token_needs_delimiter(tok))
	return is_delimiter(ch);

    /* Accept late if this char transits to an error. */
    yy_cc_t cc = char_class(character_value(ch));
    const yy_delta_row_t *row = &yy_delta[state];
    yy_state_t ns = cc < row->yy_len ? yy_next_states[row->yy_index + cc]
                                     : YY_COMMON_STATE;
    return ns == YY_ERROR_STATE;
}

static cv_t c_eat_nested_comment(obj_t cont, obj_t values)
{
    /* args: char, state, depth. */
    obj_t ch    = CAR(values);
    int   state = fixnum_value(CADR(values));
    int   depth = fixnum_value(CADDR(values));
    assert(is_null(CDDDR(values)));

    CHECK_CONDITION(!is_eof(ch),
		    &lexical, "EOF in nested comment");
    char_t c = character_value(ch);
    if (c == L'|' && state == 0)
	state = 1;
    else if (c == L'|' && state == 1) {
	/* no change */
    } else if (c == L'|' && state == 2) {
	state = 0;
	depth++;
    } else if (c == L'#' && state == 0)
	state = 2;
    else if (c == L'#' && state == 1) {
	state = 0;
	--depth;
    } else
	state = 0;
    cont_proc_t next_proc;
    obj_t       args_4th;
    if (depth) {
	/* Loop on next char. */
	next_proc = c_eat_nested_comment;
	args_4th  = MAKE_LIST(make_fixnum(state), make_fixnum(depth));
    } else {
	/* Return to read_token. */
	next_proc = c_continue_read_token;
	args_4th  = EMPTY_LIST;
    }
    /*
     * 4th c_eat_nested_comment/c_continue_read_token
     * 3rd c_peek_char
     * 2nd c_discard
     * 1st c_read_char
     */
    obj_t fourth = make_cont5(next_proc,
			      cont_cont(cont),
			      cont_env(cont),
			      cont5_arg1(cont),
			      cont5_arg2(cont));
    obj_t third  = make_cont5(c_peek_char,
			      fourth,
			      cont_env(cont),
			      MISSING_ARG,
			      args_4th);
    obj_t second = make_cont4(c_discard,
			      third,
			      cont_env(cont),
			      cont5_arg2(cont));
    obj_t first  = make_cont5(c_read_char,
			      second,
			      cont_env(cont),
			      MISSING_ARG,
			      CDR(values));
    return cv(first, cont5_arg2(cont));
}

static cv_t c_continue_read_token(obj_t cont, obj_t values)
{
    obj_t      ch     = CAR(values);
    obj_t      ctx    = cont5_arg1(cont);
    yy_state_t state  = scan_ctx_state(ctx);
    obj_t      yylval = EMPTY_LIST;

    EVAL_LOG("state=%d ch=%O", state, ch);
    if (accept_late(state, ch)) {
	token_type_t toktype = make_token(state, ctx, &yylval);
	return cv(cont_cont(cont), MAKE_LIST(make_fixnum(toktype), yylval));
    }
    if (is_eof(ch))
	return cv(cont_cont(cont),
		  MAKE_LIST(make_fixnum(TOK_EOF), make_uninitialized()));
    const yy_delta_row_t *row = &yy_delta[state];
    yy_cc_t cc = char_class(character_value(ch));
    if (cc < row->yy_len)
	state = yy_next_states[row->yy_index + cc];
    else
	state = YY_COMMON_STATE;
    CHECK_CONDITION(state != YY_ERROR_STATE,
		    &lexical, "lexical error");
    ctx = scan_ctx_advance(ctx, state, character_value(ch));
    if (state < YY_ACCEPT_COUNT) {
	yy_token_t tok = yy_accepts[state];
	if (tok == YY_ATMOSPHERE)
	    ctx = make_new_scan_ctx();
	else if (tok == YY_BEGIN_NESTED_COMMENT) {
	    /*
	     * Eat nested comment.
	     *
	     * 4th = c_eat_nested_comment
	     * 3rd = c_peek_char
	     * 2nd = c_discard
	     * 1st = c_read_char
	     */
	    ctx = make_new_scan_ctx();
	    obj_t fourth = make_cont5(c_eat_nested_comment,
				      cont_cont(cont),
				      cont_env(cont),
				      ctx,
				      cont5_arg2(cont));
	    obj_t third  = make_cont5(c_peek_char,
				      fourth,
				      cont_env(cont),
				      MISSING_ARG,
				      MAKE_LIST(make_fixnum(0),
						make_fixnum(1)));
	    obj_t second = make_cont4(c_discard,
				      third,
				      cont_env(cont),
				      cont5_arg2(cont));
	    obj_t first  = make_cont5(c_read_char,
				      second,
				      cont_env(cont),
				      MISSING_ARG,
				      CDR(values));
	    return cv(first, cont5_arg2(cont));
	} else if (accept_early(state)) {
	    /* Return (toktype yylval) after reading peeked char. */
	    token_type_t toktype = make_token(state, ctx, &yylval);
	    obj_t second = make_cont4(c_discard,
				      cont_cont(cont),
				      cont_env(cont),
				      CONS(make_fixnum(toktype),
					   CONS(yylval, EMPTY_LIST)));
	    obj_t first = make_cont5(c_read_char,
				     second,
				     cont_env(cont),
				     MISSING_ARG,
				     make_uninitialized());
	    return cv(first, cont5_arg2(cont));
	}
    }
    /*
     * 4th = c_continue_read_token
     * 3rd = c_peek_char
     * 2nd = c_discard
     * 1st = c_read_char
     */
    obj_t fourth = make_cont5(c_continue_read_token,
			      cont_cont(cont),
			      cont_env(cont),
			      ctx,
			      cont5_arg2(cont));
    obj_t third  = make_cont5(c_peek_char,
			      fourth,
			      cont_env(cont),
			      MISSING_ARG,
			      EMPTY_LIST);
    obj_t second = make_cont4(c_discard,
			      third,
			      cont_env(cont),
			      cont5_arg2(cont));
    obj_t first  = make_cont5(c_read_char,
			      second,
			      cont_env(cont),
			      MISSING_ARG,
			      CDR(values));
    return cv(first, cont5_arg2(cont));
}

/* toktype, yylval = read_token([port]) */

cv_t c_read_token(obj_t cont, obj_t values)
{
    EVAL_LOG("values=%O", values);
    obj_t second = make_cont5(c_continue_read_token,
			      cont_cont(cont),
			      cont_env(cont),
			      make_new_scan_ctx(),
			      values);
    obj_t first = make_cont5(c_peek_char,
			     second,
			     cont_env(cont),
			     MISSING_ARG,
			     values);
    return cv(first, values);
}

const char *token_name(token_type_t tok)
{
    switch (tok) {
#define CASE(x) case (x): return #x;
	CASE(TOK_EOF)
	CASE(TOK_U8)
	CASE(TOK_SIMPLE)
	CASE(TOK_ABBREV)
	CASE(TOK_COMMENT)
	CASE(TOK_BEGIN_VECTOR)
	CASE(TOK_BEGIN_BYTEVECTOR)
	CASE(TOK_LPAREN)
	CASE(TOK_RPAREN)
	CASE(TOK_PERIOD)
	CASE(TOK_LBRACKET)
	CASE(TOK_RBRACKET)
#undef CASE
    default:
	assert(false && "unknown token type");
	return NULL;
    }
}

/* spaces */
TEST_READ(L"(a b)",                     L"(a b)");
TEST_READ(L"(a\tb)",                    L"(a b)");
TEST_READ(L"(a\vb)",                    L"(a b)");
TEST_READ(L"(a\fb)",                    L"(a b)");
TEST_READ(L"(a\rb)",                    L"(a b)");
TEST_READ(L"(a\x0085"L"b)",             L"(a b)"); /* <next line> */
TEST_READ(L"(a\x2028"L"b)",             L"(a b)"); /* <line separator> */
TEST_READ(L"(a\x2029"L"b)",             L"(a b)"); /* <paragraph separator> */
TEST_READ(L"(a\x00a0"L"b)",             L"(a b)"); /* category Zs */
TEST_READ(L"(a\x2028"L"b)",             L"(a b)"); /* category Zl */
TEST_READ(L"(a\x2029"L"b)",             L"(a b)"); /* category Zp */

/* comments */
TEST_READ(L"(a;comment\nb)",            L"(a b)");
TEST_READ(L"(a;comment\rb)",            L"(a b)");
TEST_READ(L"(a;comment\r\nb)",          L"(a b)");
TEST_READ(L"(a;comment\x0085"L"b)",     L"(a b)");
TEST_READ(L"(a;comment\r\x0085"L"b)",   L"(a b)");
TEST_READ(L"(a;comment\r\nb)",          L"(a b)");
TEST_READ(L"(a;comment\x2028"L"b)",     L"(a b)");
TEST_READ(L"(a#||#b)",                  L"(a b)");
TEST_READ(L"(a#|comment|#b)",           L"(a b)");
TEST_READ(L"(a#|||#b)",                 L"(a b)");
TEST_READ(L"(a#|#||#|#b)",              L"(a b)");
TEST_READ(L"(a#|#|comment|#|#b)",       L"(a b)");
TEST_READ(L"(a#|#||#comment|#b)",       L"(a b)");

/* identifiers */
#define TEST_IDENT(name)						\
    TEST_READ(L ## #name, L ## #name);					\
    TEST_EVAL(L"(symbol? '" L ## #name L")", L"#t");
TEST_IDENT(a);
TEST_IDENT(A);
TEST_IDENT(!);
TEST_IDENT($);
TEST_IDENT(%);
TEST_IDENT(&);
TEST_IDENT(*);
TEST_IDENT(/);
TEST_IDENT(:);
TEST_IDENT(<);
TEST_IDENT(=);
TEST_IDENT(>);
TEST_IDENT(?);
TEST_IDENT(^);
TEST_IDENT(_);
TEST_IDENT(~);
TEST_IDENT(\x0102);			/* category Ll */
TEST_IDENT(\x0101);			/* category Lu */
TEST_IDENT(\x01c5);			/* category Lt */
TEST_IDENT(\x02b0);			/* category Lm */
TEST_IDENT(\x01bb);			/* category Lo */
TEST_IDENT(\x0300);			/* category Mn */
TEST_IDENT(\x2163);			/* category Nl */
TEST_IDENT(\x00bc);			/* category No */
TEST_IDENT(\x301c);			/* category Pd */
TEST_IDENT(\x2040);			/* category Pc */
TEST_IDENT(\x055e);			/* category Po */
TEST_IDENT(\x0e3f);			/* category Sc */
TEST_IDENT(\x208a);			/* category Sm */
TEST_IDENT(\x02c2);			/* category Sk */
TEST_IDENT(\x0482);			/* category So */
TEST_IDENT(\xe000);			/* category Co */
TEST_IDENT(aaA!$%&*/:<=>?^_~);
TEST_IDENT(abc\x0102);			/* category Ll */
TEST_IDENT(abc\x0101);			/* category Lu */
TEST_IDENT(abc\x01c5);			/* category Lt */
TEST_IDENT(abc\x02b0);			/* category Lm */
TEST_IDENT(abc\x01bb);			/* category Lo */
TEST_IDENT(abc\x0300);			/* category Mn */
TEST_IDENT(abc\x2163);			/* category Nl */
TEST_IDENT(abc\x00bc);			/* category No */
TEST_IDENT(abc\x301c);			/* category Pd */
TEST_IDENT(abc\x2040);			/* category Pc */
TEST_IDENT(abc\x055e);			/* category Po */
TEST_IDENT(abc\x0e3f);			/* category Sc */
TEST_IDENT(abc\x208a);			/* category Sm */
TEST_IDENT(abc\x02c2);			/* category Sk */
TEST_IDENT(abc\x0482);			/* category So */
TEST_IDENT(abc\xe000);			/* category Co */
TEST_IDENT(a123);
TEST_IDENT(abc\x0660);			/* category Nd */
TEST_IDENT(abc\x0903);			/* category Mc */
TEST_IDENT(abc\x20dd);			/* category Me */
TEST_IDENT(a+-.@);
TEST_IDENT(+);
TEST_IDENT(-);
TEST_IDENT(...);
TEST_IDENT(->);
TEST_IDENT(->abc);
TEST_READ(L"(->)",			L"(->)");
TEST_READ(L"\\x61;",			L"a");
TEST_READ(L"\\x61;\\x3BB;",		L"a\x3bb");
TEST_READ(L"\\x61;\\x00;\\x3BB;",	L"a\0\x3bb");

/* from r6rs section 4.2.4 */
TEST_IDENT(lambda);
TEST_IDENT(q);
TEST_IDENT(soup);
TEST_IDENT(list->vector);
TEST_IDENT(+);
TEST_IDENT(V17a);
TEST_IDENT(<=);
TEST_IDENT(a34kTMNs);
TEST_IDENT(->-);
TEST_IDENT(the-word-recursion-has-many-meanings);
TEST_READ(L"\\x3BB;",			L"\x3bb");

/* characters */
#define TEST_CHAR(input, expected)					\
    TEST_READ(L"#\\" input, expected);					\
    TEST_EVAL(L"(char? '#\\" input L")", L"#t");
#define TEST_CHAR_LEXICAL_EXCEPTION(input)				\
    TEST_READ(L"#\\" input, L"#<&lexical>")

/* from r6rs section 4.2.6 */
TEST_CHAR(L"a",				L"#\\a");
TEST_CHAR(L"A",				L"#\\A");
TEST_CHAR(L"(",				L"#\\(");
TEST_CHAR(L" ",				L"#\\space");
TEST_CHAR(L"nul",			L"#\\x0000");
TEST_CHAR(L"alarm",			L"#\\x0007");
TEST_CHAR(L"backspace",			L"#\\x0008");
TEST_CHAR(L"tab",			L"#\\x0009");
TEST_CHAR(L"linefeed",			L"#\\x000a");
TEST_CHAR(L"newline",			L"#\\x000a");
TEST_CHAR(L"vtab",			L"#\\x000b");
TEST_CHAR(L"page",			L"#\\x000c");
TEST_CHAR(L"return",			L"#\\x000d");
TEST_CHAR(L"esc",			L"#\\x001b");
TEST_CHAR(L"space",			L"#\\space");
TEST_CHAR(L"delete",			L"#\\x007f");
TEST_CHAR(L"xFF",			L"#\\\x00ff");
TEST_CHAR(L"x03BB",			L"#\\\x03bb");
TEST_CHAR(L"x00006587",			L"#\\\x6587");
TEST_CHAR(L"\x03bb",			L"#\\\x03bb");
TEST_CHAR_LEXICAL_EXCEPTION(L"x0001z");
TEST_CHAR_LEXICAL_EXCEPTION(L"\x03bbx");
TEST_CHAR_LEXICAL_EXCEPTION(L"alarmx");
TEST_READ(L"#\\alarm x",		L"#\\x0007");
TEST_CHAR_LEXICAL_EXCEPTION(L"Alarm");
TEST_CHAR_LEXICAL_EXCEPTION(L"alert");
TEST_CHAR(L"xA",			L"#\\x000a");
TEST_CHAR(L"xFF",			L"#\\\x00ff");
TEST_CHAR(L"xff",			L"#\\\x00ff");
TEST_READ(L"#\\x ff",			L"#\\x");
TEST_READ(L"#\\x(ff)",			L"#\\x");
TEST_CHAR_LEXICAL_EXCEPTION(L"(x)");
TEST_CHAR_LEXICAL_EXCEPTION(L"(x");
TEST_READ(L"#\\((x)",			L"#\\(");
TEST_CHAR_LEXICAL_EXCEPTION(L"x00110000");
TEST_CHAR(L"x000000001",		L"#\\x0001");
TEST_CHAR_LEXICAL_EXCEPTION(L"xD800");

/* strings */
#define TEST_STRING(input, expected)					\
    TEST_READ(L ## input, L ## expected);				\
    TEST_EVAL(L"(string? " L ## input L")", L"#t");
#define TEST_STRING_LEXICAL_EXCEPTION(input)				\
    TEST_READ(L ## input, L"#<&lexical>")

TEST_STRING("\"foo\"", "\"foo\"");
TEST_STRING("\"\"", "\"\"");
TEST_STRING("\"\\a\\b\\t\\n\\v\\f\\r\\\"\\\\\"", "\"\a\b\t\n\v\f\r\"\\\"");
TEST_STRING("\"a\\x3bb;b\"",		"\"a\x03bb" L"b\"");
TEST_STRING("\"a \\  \n  b\"",		"\"a b\"");
TEST_STRING("\"a \\  \r  b\"",		"\"a b\"");
TEST_STRING("\"a \\  \r\n  b\"",	"\"a b\"");
TEST_STRING("\"a \\  \r\n  b\"",	"\"a b\"");
TEST_STRING("\"a \\\t\r\n  b\"",	"\"a b\"");
TEST_STRING("\"a \\  \r\n\tb\"",	"\"a b\"");
TEST_STRING("\"a \\  \x85  b\"",	"\"a b\"");
TEST_STRING("\"a \\  \r\x85  b\"",	"\"a b\"");
TEST_STRING("\"a \\  \x2028  b\"",	"\"a b\"");

/* from r6rs section 4.2.7 */
TEST_STRING("\"abc\"",			"\"abc\"");
TEST_STRING("\"\\x41;bc\"",		"\"Abc\"");
TEST_STRING("\"\\x41; bc\"",		"\"A bc\"");
TEST_STRING("\"\\x41bc;\"",		"\"\x41bc\"");
TEST_STRING_LEXICAL_EXCEPTION("\"\\x41\"");
TEST_STRING_LEXICAL_EXCEPTION("\"\\x;\"");
TEST_STRING_LEXICAL_EXCEPTION("\"\\x41bx;\"");
TEST_STRING("\"\\x00000041;\"",		"\"A\"");
TEST_STRING("\"\\x0010ffff;\"",		"\"\x10ffff\"");
TEST_STRING_LEXICAL_EXCEPTION("\"\\x00000001\"");
TEST_STRING_LEXICAL_EXCEPTION("\"\\xD800;\"");
TEST_STRING("\"A\nbc\"",			"\"A\nbc\"");

/* numbers */
#define TEST_NUMBER(input, expected)					\
    TEST_READ(L ## #input, L ## #expected);				\
    TEST_EVAL(L"(integer? " L ## #input L")", L"#t");
TEST_NUMBER(0,       0);
TEST_NUMBER(+12,    12);
TEST_NUMBER(-23,   -23);

TEST_NUMBER(#i0,   0);
TEST_NUMBER(#I0,   0);
TEST_NUMBER(#e0,   0);
TEST_NUMBER(#E0,   0);
TEST_NUMBER(#b101, 5);
TEST_NUMBER(#i0,   0);
TEST_NUMBER(#i0,   0);
TEST_NUMBER(#i0,   0);
TEST_NUMBER(#i0,   0);
TEST_READ(L"#i0",                               L"0");
TEST_READ(L"#I0",                               L"0");
TEST_READ(L"#e0",                               L"0");
TEST_READ(L"#E0",                               L"0");
TEST_READ(L"#b101",                             L"5");
TEST_READ(L"#o77",                              L"63");
TEST_READ(L"#e#b101",                           L"5");
//TEST_READ(L"0.1",                               L"0.1");
//TEST_READ(L"#e0.1",                             L"1/10");
