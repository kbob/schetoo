#include <setjmp.h>

extern jmp_buf buf;
extern int get_ready_to_jump(const char *);

static inline int f(int a)
{
    if (a & 0x1)
	longjmp(buf, get_ready_to_jump("foo"));
    return a >> 1;
}

int g(int x)
{
    return f(x) + f(x + 2);
}
