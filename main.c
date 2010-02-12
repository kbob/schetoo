#include <stdio.h>
#include <string.h>

#include "env.h"
#include "eval.h"
#include "except.h"
#include "heap.h"
#include "io.h"
#include "print.h"
#include "prim.h"
#include "read.h"
#include "record.h"
#include "roots.h"
#include "test.h"
#include "types.h"

// XXX exception handlers are missing when read_stream() runs.
static void repl(void)
{
    instream_t *in = make_readline_instream();
    outstream_t *out = make_file_outstream(stdout);
    obj_t expr;
    /* make a silly environment for testing. */
    obj_t env = root_environment();
    while (read_stream(in, &expr))
	print(core_eval(expr, env), out);
    printf("\n");
}

int main(int argc, char *argv[])
{
    set_program_name(argv[0]);
    init_heap();
    init_roots();
    init_records();
    register_primitives();
    if (argc == 2 && !strcmp(argv[1], "-t"))
	self_test();
    else
	repl();
    return 0;
}
