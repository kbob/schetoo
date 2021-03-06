#include "charbuf.h"
#include "prim.h"
#include "obj_eof.h"
#include "obj_port.h"
#include "read.h"
#include "record.h"
#include "roots.h"
#include "types.h"

#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wchar.h>

DEFINE_STATIC_RECORD_TYPE(io_error, L"&i/o", &error, 0) = {
    { FM_END }
};

DEFINE_STATIC_RECORD_TYPE(io_read, L"&i/o-read", &io_error, 0) = {
    { FM_END }
};

ROOT(standard_input);

static char *readline_prompt = "> ";

static obj_t readline_read(obj_t port)
{
    static char *line;
    assert(is_port(port));
    if (line == NULL) {
	line = readline(readline_prompt);
	if (line == NULL) {
	    port_set_text_buffer(port, make_eof());
	    putchar('\n');
	    fflush(stdout);
	    return port;
	}
	if (*line)
	    add_history(line);
    }
    mbstate_t mbstate;
    memset(&mbstate, '\0', sizeof mbstate);
    size_t n, len = strlen(line);
    const char *p = line;
    charbuf_t buf;
    wchar_t wc;
    init_charbuf(&buf);
    while ((n = mbrtowc(&wc, p, len, &mbstate)) > 0) {
	charbuf_append_char(&buf, wc);
	p += n;
	len -= n;
    }
    charbuf_append_char(&buf, L'\n');
    port_set_text_buffer(port, charbuf_make_string(&buf));
    port_set_text_pos(port, 0);
    free(line);
    line = NULL;
    SIDE_EFFECT();
    return port;
}

static obj_t make_readline_port(void)
{
    obj_t readline_read_name = make_symbol_from_C_str(L"readline-read");
    obj_t readline_read_proc = make_C_procedure(readline_read,
						readline_read_name,
						COMPILE_INTERVAL(1),
						EMPTY_LIST);
    return make_port(PORT_TEXTUAL | PORT_INPUT,
		     -1,
		     readline_read_proc,
		     FALSE_OBJ,
		     FALSE_OBJ,
		     FALSE_OBJ,
		     make_uninitialized(),
		     FALSE_OBJ);
}

static size_t decode(charbuf_t *cbufp, const byte_t *bytes, size_t len)
{
    size_t i, j;
    mbstate_t mbstate;
    memset(&mbstate, '\0', sizeof mbstate);
    for (i = 0; i < len; i += j) {
	wchar_t wc;
	j = mbrtowc(&wc, (const char *)(bytes + i), len - i, &mbstate);
	if (j == (size_t)-1)
	    THROW(&io_read, "illegal character");
	if (j == (size_t)-2)
	    break;
	charbuf_append_char(cbufp, wc);
    }
    return i;
}

static obj_t text_file_read(obj_t port)
{
    obj_t  bbuf  = port_binary_buffer(port);
    size_t blen  = port_binary_len(port);
    size_t bsize = bytevector_len(bbuf);
    charbuf_t cbuf;
    init_charbuf_size(&cbuf, bsize);
    while (1) {
	byte_t *bytes = bytevector_addr(bbuf);
	size_t bpos   = port_binary_pos(port);
	size_t nbytes = decode(&cbuf, bytes + bpos, blen - bpos);
	if (nbytes) {
	    // N.B., must not fail.
	    port_set_text_buffer(port, charbuf_make_string(&cbuf));
	    bpos += nbytes;
	    memmove(bytes, bytes + bpos, blen - bpos);
	    port_set_binary_pos(port, 0);
	    port_set_binary_len(port, blen - bpos);
	    port_set_buffer_offset(port, port_buffer_offset(port) + bpos);
	    return port;
	}
	int fd = port_fd(port);
	ssize_t nb = read(fd, bytes + blen, bsize - blen);
	SIDE_EFFECT();
	if (nb < 0)
	    THROW(&io_read, "read failed");
	if (nb == 0) {
	    port_set_text_buffer(port, make_eof());
	    return port;
	}
	blen += nb;
    }
}

static obj_t file_close(obj_t port)
{
    assert(is_port(port));
    (void)close(port_fd(port));
    return port;
}

static obj_t make_text_file_input_port(int fd, bool close)
{
    struct stat st;
    if (fstat(fd, &st) < 0)
	THROW(&io_error, "stat failed");
    size_t blksize = st.st_blksize;
    obj_t binary_buffer = make_bytevector_uninitialized(blksize);
    obj_t file_read_name = make_symbol_from_C_str(L"text-file-read");
    obj_t file_read_proc = make_C_procedure(text_file_read,
					    file_read_name,
					    COMPILE_INTERVAL(1),
					    EMPTY_LIST);
    obj_t file_close_proc = FALSE_OBJ;
    if (close) {
	obj_t file_close_name = make_symbol_from_C_str(L"file-close");
	file_close_proc = make_C_procedure(file_close,
					   file_close_name,
					   COMPILE_INTERVAL(1),
					   EMPTY_LIST);
    }
    return make_port(PORT_TEXTUAL | PORT_INPUT,
		     fd,
		     file_read_proc,
		     FALSE_OBJ,		/* write proc */
		     FALSE_OBJ,		/* seek proc */
		     file_close_proc,
		     make_uninitialized(), /* text buffer */
		     binary_buffer);	/* binary buffer */
}

static obj_t current_stdin(void)
{
    if (is_uninitialized(standard_input)) {
	int ifd = fileno(stdin), ofd = fileno(stdout);
	if (isatty(ifd) && isatty(ofd))
	    standard_input = make_readline_port();
	else
	    standard_input = make_text_file_input_port(ifd, false);
    }
    return standard_input;
}

DEFINE_PROC(L"eof-object", 0)(void)
{
    return make_eof();
}

DEFINE_PROC(L"eof-object?", 1)(obj_t obj)
{
    return make_boolean(is_eof(obj));
}

DEFINE_STATIC_PROC(string_read, NULL, 1)(obj_t port)
{
    EVAL_LOG("port=%O", port);
    port_set_buffer_offset(port, string_len(port_text_buffer(port)));
    port_set_text_pos(port, 0);
    port_set_text_buffer(port, make_eof());
    return port;
}

DEFINE_PROC(L"open-string-input-port", 1)(obj_t string)
{
    obj_t name = make_symbol_from_C_str(L"string_read_proc");
    obj_t string_read_proc = make_C_procedure(string_read,
					      name,
					      COMPILE_INTERVAL(1),
					      EMPTY_LIST);
    return make_port(PORT_TEXTUAL | PORT_INPUT | PORT_SEEKABLE,
		     -1, 
		     string_read_proc,
		     FALSE_OBJ,
		     FALSE_OBJ,
		     FALSE_OBJ,
		     string,
		     FALSE_OBJ);
}

static cv_t c_continue_read_char(obj_t cont, obj_t values)
{
    // (continue-read-char ch)
    assert(is_cont5(cont));
    obj_t port = cont5_arg1(cont);
    obj_t saved_values = cont5_arg2(cont);
    obj_t ch = CAR(values);
    EVAL_LOG("port=%O ch=%O", port, ch);
    CHECK(is_port(port), "must be port");
    obj_t caller_values = CONS(ch, saved_values);
    obj_t ret = cont_cont(cont);
    if (!is_eof(ch))
	port_set_text_pos(port, port_text_pos(port) + 1);
    return cv(ret, caller_values);
}

DEFINE_EXTERN_RAW_PROC(c_read_char, L"read-char")(obj_t cont, obj_t values)
{
    assert(is_cont5(cont));
    CHECK(is_null(values) || is_null(CDR(values)), "too many arguments");
    obj_t port = is_null(values) ? current_stdin() : CAR(values);
    EVAL_LOG("values=%O port=%O", values, port);
    obj_t second = make_cont5(c_continue_read_char,
			      cont_cont(cont),
			      cont_env(cont),
			      port,
			      cont5_arg2(cont));
    obj_t first = make_cont5(c_peek_char,
			     second,
			     cont_env(cont),
			     cont5_arg1(cont),
			     EMPTY_LIST);
    return cv(first, values);
}

DEFINE_EXTERN_RAW_PROC(c_peek_char, L"peek-char")(obj_t cont, obj_t values)
{
    assert(is_cont5(cont));
    CHECK(is_null(values) || is_null(CDR(values)), "too many arguments");
    obj_t saved_values = cont5_arg2(cont);
    obj_t port = is_null(values) ? current_stdin() : CAR(values);
    EVAL_LOG("values=%O port=%O", values, port);
    CHECK(is_port(port), "must be port");
    obj_t rbuf = port_text_buffer(port);
    if (!is_uninitialized(rbuf)) {
	if (is_eof(rbuf))
	    return cv(cont_cont(cont), CONS(rbuf, saved_values));
	size_t rpos = port_text_pos(port);
	size_t rblen = string_len(rbuf);
	if (rpos < rblen)
	    return cv(cont_cont(cont),
		      CONS(make_character(string_value(rbuf)[rpos]),
			   saved_values));
    }
    /* Can't read from current buffer.  Fill buffer and try again. */

    return cv(make_cont5(c_apply_proc,
			 cont,
			 cont_env(cont),
			 port_read_proc(port),
			 EMPTY_LIST),
	      CONS(port, EMPTY_LIST));
}

DEFINE_RAW_PROC(L"read")(obj_t cont, obj_t values)
{
    return c_read(cont, values);
}
