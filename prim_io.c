#include "charbuf.h"
#include "prim.h"
#include "obj_eof.h"
#include "read.h"
#include "roots.h"
#include "types.h"

#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>

static bool     is_initialized = false;
static bool     using_readline;
static size_t   readline_pos;
static char    *readline_prompt = "] ";

ROOT_CONSTRUCTOR(readline_line)
{
    return make_undefined();
}

extern obj_t peek_char(obj_t);

DEFINE_PROC(L"eof-object", 0)(void)
{
    return make_eof();
}

DEFINE_PROC(L"eof-object?", 1)(obj_t obj)
{
    return make_boolean(is_eof(obj));
}

DEFINE_EXTERN_PROC(read_char, L"read-char", 0-1)(obj_t textual_input_port)
{
    obj_t ch = peek_char(textual_input_port);
    // N.B., must not raise any exceptions.
    if (!is_eof(ch)) {
	if (using_readline) {
	    if (is_string(readline_line))
		readline_pos++;
	} else
	    (void)getwchar();
    }
    return ch;
}

DEFINE_EXTERN_PROC(peek_char, L"peek-char", 0-1)(obj_t textual_input_port)
{
    EVAL_LOG("peek-char port=%O", textual_input_port);
    if (!is_initialized) {
	using_readline = isatty(fileno(stdin)) && isatty(fileno(stdout));
	is_initialized = true;
    }
    if (using_readline) {
	wchar_t wc;
	if (is_string(readline_line) &&
	    readline_pos > string_len(readline_line)) {
	    readline_line = make_undefined();
	}
	if (is_undefined(readline_line)) {
	    char *line = readline(readline_prompt);
	    if (!line)
		return readline_line = make_eof();
	    if (*line)
		add_history(line);
	    mbstate_t mbstate;
	    memset(&mbstate, '\0', sizeof mbstate);
	    size_t n, len = strlen(line);
	    const char *p = line;
	    charbuf_t buf;
	    init_charbuf(&buf, L"");
	    while ((n = mbrtowc(&wc, p, len, &mbstate)) > 0) {
		charbuf_append_char(&buf, wc);
		p += n;
		len -= n;
	    }
	    free(line);
	    readline_line = charbuf_make_string(&buf);
	    readline_pos = 0;
	}
	if (is_eof(readline_line))
	    return readline_line;	/* EOF is sticky. */
	if (readline_pos < string_len(readline_line))
	    wc = string_value(readline_line)[readline_pos];
	else
	    wc = L'\n';
	return make_character(wc);
    } else {
	wint_t wc = ungetwc(getwchar(), stdin);
	if (wc == WEOF)
	    return make_eof();
	else
	    return make_character(wc);
    }
}

DEFINE_RAW_PROC(L"read")(obj_t cont, obj_t values)
{
    return c_read(cont, values);
}
