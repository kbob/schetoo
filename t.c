#include <pthread.h>
#include <setjmp.h>
#include <stdio.h>

#if 0
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
#endif

void *start(void *unused)
{
    pthread_key_t k;
    pthread_key_create(&k, NULL);
    printf("pthread_self = %lld\n", (long long)pthread_self());
    printf("         key = %lld\n", (long long) k);
}

int main(void)
{
    start(NULL);
    pthread_t t;
    pthread_create(&t, NULL, start, NULL);
    pthread_join(t, NULL);
    return 0;
}
