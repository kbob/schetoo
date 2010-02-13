#include "oprintf.h"

/* Register new printf handlers to print objects. */
/* Not used until the heap is initialized. */

#include <assert.h>
#include <printf.h>
#include <stdarg.h>

#include "io.h"
#include "print.h"

static int print_obj(FILE *stream,
		     const struct printf_info *info,
		     const void *const *args)
{
    outstream_t *out = make_file_outstream(stream);
    long a = outstream_pos(out);
    if (info->prec != -1 && setjmp(*outstream_set_limit(out, info->prec))) {
	outstream_clear_limit(out);
	outstream_printf(out, L"...");
    } else {
	obj_t obj = *(obj_t *)args[0];
	princ(obj, out);
    }
    long b = outstream_pos(out);
    delete_outstream(out);
    return b - a;
}

static int obj_arginfo(const struct printf_info *info, size_t n, int *argtypes, int *size)
{
    if (n > 0) {
	argtypes[0] = PA_POINTER;
	size[0] = sizeof (obj_t);
    }
    return 1;
}

__attribute__((constructor))
static void extend_printf(void)
{
    register_printf_specifier('O', print_obj, obj_arginfo);
}

int oprintf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int r = vprintf(format, ap);
    va_end(ap);
    return r;
}

int ofprintf(FILE *f, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int r = vfprintf(f, format, ap);
    va_end(ap);
    return r;
}
