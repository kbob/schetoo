#include <stdio.h>

typedef enum bool { false = 0, true = 1 } bool;

typedef struct foo {
    bool variadic;
    bool infinite;
    int lower;
    int upper;
} foo_t;

#define dcl(name, args)							\
    foo_t name = { !!(0 * args + 0),					\
		   args + 1 < args + 0,					\
		   (args + 0) - (0 * args + 0),				\
		   (0 * args + 0) ?					\
                       -(0 * args + 0) :				\
                       (args + 0) - (0 * args + 0)			\
		 }
	       
dcl(f0, 0);
dcl(f1, 1);
dcl(f2, 2);
dcl(f3, 3);

dcl(v00, 0-0);
dcl(v01, 0-1);
dcl(v02, 0-2);
dcl(v13, 1-3);
dcl(v23, 2-3);

dcl(i0, 0-);
dcl(i1, 1-);
dcl(i2, 2-);
dcl(i3, 3-);

void show(const char *name, foo_t *p)
{
    printf("%s { variadic = %d, infinite = %d, lower = %d, upper = %d }\n",
	   name, p->variadic, p->infinite, p->lower, p->upper);
}

int main()
{
    show("f0", &f0);
    show("f1", &f1);
    show("f2", &f2);
    show("f3", &f3);
    putchar('\n');
    show("v00", &v00);
    show("v01", &v01);
    show("v02", &v02);
    show("v13", &v13);
    show("v23", &v23);
    putchar('\n');
    show("i0", &i0);
    show("i1", &i1);
    show("i2", &i2);
    show("i3", &i3);
    return 0;
}
