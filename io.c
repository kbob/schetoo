#include "io.h"

#include <assert.h>
#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdarg.h>
#include <stdlib.h>

#include "bool.h"
#include "except.h"

typedef void   (*in_delete_proc_t)(instream_t *);
typedef wint_t (*in_getwc_proc_t)(instream_t *);
typedef wint_t (*in_ungetwc_proc_t)(wint_t, instream_t *);
typedef void   (*out_delete_proc_t)(outstream_t *);
typedef wint_t (*out_putwc_proc_t)(wchar_t, outstream_t *);
typedef int    (*out_vprintf_proc_t)(outstream_t *,
				     const wchar_t *fmt, va_list);

struct instream {
    in_delete_proc_t   in_delete;
    in_getwc_proc_t    in_getwc;
    in_ungetwc_proc_t  in_ungetwc;
};

typedef struct file_instream {
    instream_t         fi_instream;
    FILE              *fi_file;
} file_instream_t;

typedef struct readline_instream {
    instream_t         ri_instream;
    wchar_t           *ri_linebuf;
    size_t             ri_pos;
    wchar_t            ri_ungot;
    mbstate_t          ri_ps;
} readline_instream_t;

typedef struct string_instream {
    instream_t         si_instream;
    const wchar_t     *si_inbuf;
    size_t             si_size;
    size_t             si_pos;
} string_instream_t;

struct outstream {
    out_delete_proc_t  out_delete;
    out_putwc_proc_t   out_putwc;
    out_vprintf_proc_t out_vprintf;
    off_t              out_pos;
    off_t              out_limit;
    jmp_buf            out_jmpbuf;
};

typedef struct file_outstream {
    outstream_t        fo_outstream;
    FILE              *fo_file;
} file_outstream_t;

typedef struct string_outstream {
    outstream_t        so_outstream;
    wchar_t           *so_outbuf;
    size_t             so_size;
    size_t             so_pos;
} string_outstream_t;

static void init_instream(instream_t *ip,
			  in_delete_proc_t del,
			  in_getwc_proc_t get,
			  in_ungetwc_proc_t unget)
{
    ip->in_delete = del;
    ip->in_getwc = get;
    ip->in_ungetwc = unget;
}

static void init_outstream(outstream_t       *out,
			   out_delete_proc_t  del,
			   out_putwc_proc_t   put,
			   out_vprintf_proc_t vprintf)
{
    out->out_delete = del;
    out->out_putwc = put;
    out->out_vprintf = vprintf;
    out->out_pos = 0;
    out->out_limit = -1;
    memset(&out->out_jmpbuf, '\0', sizeof out->out_jmpbuf);
}

static void instream_delete(instream_t *in)
{
    free(in);
}

static void outstream_delete(outstream_t *out)
{
    free(out);
}

/* file instream methods */

static wint_t file_getwc(instream_t *ip)
{
    file_instream_t *fp = (file_instream_t *)ip;
    return fgetwc(fp->fi_file);
}

static wint_t file_ungetwc(wint_t wc, instream_t *ip)
{
    file_instream_t *fp = (file_instream_t *)ip;
    return ungetwc(wc, fp->fi_file);
}

instream_t *make_file_instream(FILE *f)
{
    file_instream_t *fp = malloc(sizeof *fp);
    instream_t *ip = &fp->fi_instream;
    init_instream(ip, instream_delete, file_getwc, file_ungetwc);
    fp->fi_file = f;
    return ip;
}

/* readline instream methods */

static wint_t readline_getwc(instream_t *ip)
{
    readline_instream_t *rp = (readline_instream_t *)ip;
    wint_t wc;
    size_t line_len;
    size_t nb;

    if (rp->ri_ungot != WEOF) {
	wc = rp->ri_ungot;
	rp->ri_ungot = WEOF;
	return wc;
    }
    if (rp->ri_linebuf == NULL) {
	const char *linebuf = readline("> ");
	const char *tmp = linebuf;
	mbstate_t mbstate;
	if (!linebuf) {
	    return WEOF;
	}
	memset(&mbstate, '\0', sizeof mbstate);
	line_len = mbsrtowcs(NULL, &linebuf, 0, &mbstate);
	assert(line_len >= 0);
	rp->ri_linebuf = malloc((line_len + 1) * sizeof *rp->ri_linebuf);
	tmp = linebuf;
	nb = mbsrtowcs(rp->ri_linebuf, &tmp, line_len + 1, &mbstate);
	assert(nb >= 0);
	rp->ri_pos = 0;
    }
    line_len = wcslen(rp->ri_linebuf);
    if (rp->ri_pos < line_len) {
	wc = rp->ri_linebuf[rp->ri_pos++];
    } else {
	free(rp->ri_linebuf);
	rp->ri_linebuf = NULL;
	wc = L'\n';
    }
    return wc;
}

static wint_t readline_ungetwc(wint_t wc, instream_t *ip)
{
    readline_instream_t *rp = (readline_instream_t *)ip;
    assert(rp->ri_ungot == WEOF);
    assert(wc != WEOF);
    rp->ri_ungot = wc;
    return wc;
}

instream_t *make_readline_instream(void)
{
    readline_instream_t *rp = malloc(sizeof *rp);
    instream_t *ip = &rp->ri_instream;
    init_instream(ip, instream_delete, readline_getwc, readline_ungetwc);
    rp->ri_linebuf = NULL;
    rp->ri_pos = 0;
    rp->ri_ungot = WEOF;
    return ip;
}

/* string instream methods */

static wint_t string_getwc(instream_t *ip)
{
    string_instream_t *sp = (string_instream_t *)ip;
    if (sp->si_pos >= sp->si_size)
	return WEOF;
    return sp->si_inbuf[sp->si_pos++];
}

static wint_t string_ungetwc(wint_t wc, instream_t *ip)
{
    string_instream_t *sp = (string_instream_t *)ip;
    assert(sp->si_pos > 0);
    assert(wc == sp->si_inbuf[sp->si_pos - 1]);
    --sp->si_pos;
    return wc;
}

instream_t *make_string_instream(const wchar_t *str, size_t size)
{
    string_instream_t *sp = malloc(sizeof *sp);
    instream_t *ip = &sp->si_instream;
    init_instream(ip, instream_delete, string_getwc, string_ungetwc);
    sp->si_inbuf = str;
    sp->si_pos = 0;
    sp->si_size = size;
    return ip;
}

/* file outstream methods */

static wint_t file_putwc(wchar_t wc, outstream_t *out)
{
    file_outstream_t *fp = (file_outstream_t *)out;
    if (fwide(fp->fo_file, 0) > 0) {
	return fputwc(wc, fp->fo_file);
    } else {
	char buf[MB_LEN_MAX];
	int r, nb = wctomb(buf, wc);
	if (nb < 0)
	    return WEOF;
	r = fwrite(buf, nb, 1, fp->fo_file);
	return (r == 1) ? wc : WEOF;
    }
}

static int file_vprintf(outstream_t *out, const wchar_t *fmt, va_list ap)
{
    file_outstream_t *fp = (file_outstream_t *)out;
    if (fwide(fp->fo_file, 0) > 0)
	return vfwprintf(fp->fo_file, fmt, ap);
    size_t max = 64;
    while (true) {
	//wchar_t *buf = alloca(max * sizeof *buf);
	wchar_t buf[max];
	int i, n = vswprintf(buf, max, fmt, ap);
	if (n >= 0) {
	    for (i = 0; i < n; i++)
		if (file_putwc(buf[i], out) == WEOF)
		    return WEOF;
	    return n;
	}
	max *= 2;
    }
}

outstream_t *make_file_outstream(FILE *f)
{
    file_outstream_t *fp = malloc(sizeof *fp);
    outstream_t *out = &fp->fo_outstream;
    init_outstream(out, outstream_delete, file_putwc, file_vprintf);
    fp->fo_file = f;
    return out;
}

/* string outstream methods */

static wint_t string_putwc(wchar_t wc, outstream_t *out)
{
    string_outstream_t *sp = (string_outstream_t *)out;
    if (sp->so_pos >= sp->so_size - 1)
	return WEOF;
    sp->so_outbuf[sp->so_pos++] = wc;
    sp->so_outbuf[sp->so_pos] = L'\0';
    return wc;
}

static int string_vprintf(outstream_t *out, const wchar_t *fmt, va_list ap)
{
    string_outstream_t *sp = (string_outstream_t *)out;
    wchar_t *s = sp->so_outbuf + sp->so_pos;
    size_t max = sp->so_size - sp->so_pos;
    int n = vswprintf(s, max, fmt, ap);
    if (n >= max)
	n = -1;
    if (n > 0)
	sp->so_pos += n;
    else
	sp->so_outbuf[sp->so_pos] = L'0';
    return n;
}

outstream_t *make_string_outstream(wchar_t *outbuf, size_t size)
{
    string_outstream_t *sp = malloc(sizeof *sp);
    outstream_t *out = &sp->so_outstream;
    init_outstream(out, outstream_delete, string_putwc, string_vprintf);
    sp->so_outbuf = outbuf;
    sp->so_pos = 0;
    sp->so_size = size;
    return out;
}

/* generic methods */

void delete_instream(instream_t *in)
{
    in->in_delete(in);
}

wint_t instream_getwc(instream_t *ip)
{
    return ip->in_getwc(ip);
}

wint_t instream_ungetwc(wint_t wc, instream_t *ip)
{
    return ip->in_ungetwc(wc, ip);
}

void delete_outstream(outstream_t *out)
{
    out->out_delete(out);
}

wint_t outstream_putwc(wchar_t wc, outstream_t *out)
{
    out->out_pos++;
    wint_t r = out->out_putwc(wc, out);
    if (out->out_limit >= 0 && out->out_pos >= out->out_limit) {
	out->out_limit = -1;
	longjmp(out->out_jmpbuf, 1);
    }
    return r;
}

int outstream_printf(outstream_t *out, const wchar_t *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n = out->out_vprintf(out, fmt, ap);
    va_end(ap);
    out->out_pos += n;
    if (out->out_limit >= 0 && out->out_pos >= out->out_limit) {
	out->out_limit = -1;
	longjmp(out->out_jmpbuf, 1);
    }
    return n;
}

off_t outstream_pos(outstream_t *out)
{
    return out->out_pos;
}

jmp_buf *outstream_set_limit(outstream_t *out, off_t limit)
{
    out->out_limit = out->out_pos + limit;
    return &out->out_jmpbuf;
}

void outstream_clear_limit(outstream_t *out)
{
    out->out_limit = -1;
}
