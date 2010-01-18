#include "except.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

condition_type_t condition		    = { "condition" };
condition_type_t warning		    = { "warning" };
condition_type_t serious		    = { "serious" };
condition_type_t message		    = { "message" };
condition_type_t irritants		    = { "irritants" };
condition_type_t who			    = { "who" };
condition_type_t error			    = { "error" };
condition_type_t violation		    = { "violation" };
condition_type_t assertion		    = { "assertion" };
condition_type_t non_continuable	    = { "non_continuable" };
condition_type_t implementation_restriction = { "implementation_restriction" };
condition_type_t lexical		    = { "lexical" };
condition_type_t syntax			    = { "syntax" };
condition_type_t undefined		    = { "undefined" };

static const char *program_name;
static const char *program_short_name;

void assertion_failed(const char *file,
		      int	 line,
		      const char *fn,
		      const char *expr)
{
    fprintf(stderr, "%s: %s:%d: %s: Assertion `%s' failed.\n",
	    program_short_name, file, line, fn, expr);
    abort();
}

void raise(condition_type_t *ct, obj_t obj, const char *msg, ...)
{
    fprintf(stderr, "%s: condition &%s: %s\n",
	    program_short_name, ct->ct_name, msg);
    abort();
}
    
void raise_continuable(condition_type_t *ct, obj_t obj, const char *msg)
{
    // XXX implement
    raise(ct, obj, msg);
}

void set_program_name(const char *path)
{
    program_name = path;
    program_short_name = strrchr(path, '/');
    if (program_short_name)
	program_short_name++;
    else
	program_short_name = path;
}
