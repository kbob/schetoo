#include <stdio.h>

template <class T>
int precision(void)
{
    T A[100];
    int n = 0;
    T x = 1.0;
    for (n = 0 ; n < 100; n++, x /= 2.0)
	A[n] = T(1.0) + x;

    for (n = 0; n < 100; n++)
	if (A[n] == T(1.0))
	    return n;
}

#define PRINT_PRECISION(type) \
    (printf(#type ": %d of %d bits\n", precision<type>(), 8 * sizeof (type)))

int main()
{
    PRINT_PRECISION(float);
    PRINT_PRECISION(double);
    PRINT_PRECISION(long double);
    return 0;
}
