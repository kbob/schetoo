#include <string.h>

#include "except.h"
#include "heap.h"
#include "prim.h"
#include "repl.h"
#include "record.h"
#include "roots.h"
#include "test.h"

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
	core_repl();
    return 0;
}
