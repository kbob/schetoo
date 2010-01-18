#include <stdio.h>

#include "mem.h"
#include "except.h"
#include "io.h"
#include "print.h"
#include "read.h"
#include "roots.h"

static void repl(void)
{
    instream_t *in = make_readline_instream();
    outstream_t *out = make_file_outstream(stdout);
    obj_t expr;
    while (read_stream(in, &expr))
	print(expr, out);
    printf("\n");
}

int main(int argc, char *argv[])
{
    set_program_name(argv[0]);
    init_heap();
    init_roots();
    repl();
    return 0;
}
