#include <stdio.h>

#include "mem.h"
#include "except.h"
#include "obj_boolean.h"
#include "obj_bytevector.h"
#include "record.h"

int main(int argc, char *argv[])
{
    set_program_name(argv[0]);
    init_heap();
    obj_t f = make_boolean(0);
    printf("f=%p\n", f);
    obj_t t = make_boolean(1);
    printf("t=%p\n", t);
    obj_t p = make_bytevector(3, 'k');
    printf("p=%p\n", p);
    bytevector_set(p, 1, 'j');
    printf("p[0] = 0%o = '%c'\n", bytevector_get(p, 0), bytevector_get(p, 0));
    printf("p[1] = 0%o = '%c'\n", bytevector_get(p, 1), bytevector_get(p, 1));
    return 0;
}
