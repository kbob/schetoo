#include "except.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "list.h"
#include "low_ex.h"
#include "obj_pair.h"
#include "obj_record.h"
#include "obj_rtd.h"
#include "obj_string.h"
#include "obj_symbol.h"
#include "oprintf.h"

static const char *prog_name;
static const char *prog_short_name;

void throw(obj_t *ct, const wchar_t *msg, ...)
{
#if 0
    {
	fprintf(stderr, "%s: condition %ls: %ls",
		program_short_name(),
		string_value(symbol_name(rtd_name(*ct))), msg);
	va_list ap1;
	va_start(ap1, msg);
	obj_t irritant;
	while ((irritant = va_arg(ap1, obj_t)) != END_OF_ARGS)
	    ofprintf(stderr, " %O", irritant);
	va_end(ap1);
	fprintf(stderr, "\n");
    }
#endif
    /*
     * Make &message condition.
     * Make irritants.
     * Pack them all up in a compound condition.
     * Longjmp out.
     */

    obj_t primary_ex = MAKE_RECORD(*ct);
    obj_t msg_str = make_string_from_chars(msg, wcslen(msg));
    obj_t msg_ex = MAKE_RECORD(message, msg_str);
    obj_t ex;
    obj_t irritant;
    obj_t irr_list = EMPTY_LIST;
    va_list ap;
    va_start(ap, msg);
    while ((irritant = va_arg(ap, obj_t)) != END_OF_ARGS)
	irr_list = CONS(irritant, irr_list);
    va_end(ap);
    if (irr_list != EMPTY_LIST) {
	irr_list = reverse_list(irr_list);
	obj_t irr_ex = MAKE_RECORD(irritants, irr_list);
	ex = MAKE_COMPOUND_CONDITION(primary_ex, msg_ex, irr_ex);
    } else
	ex = MAKE_COMPOUND_CONDITION(primary_ex, msg_ex);
    send_exception(ex);
}
    
void set_program_name(const char *path)
{
    prog_name = path;
    prog_short_name = strrchr(path, '/');
    if (prog_short_name)
	prog_short_name++;
    else
	prog_short_name = path;
}

const char *program_short_name(void)
{
    return prog_short_name;
}
