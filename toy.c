#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>

typedef uint8_t cc_t;
typedef uint8_t q_t;

#define Q_COUNT ((q_t)44)
#define CC_COUNT ((cc_t)176)

//const size_t q_count = 44;
//const size_t cc_count = 176;

static cc_t cc[256];
static q_t delta[Q_COUNT][CC_COUNT];

static uint8_t cc[256];

int char_class(int c)
{
    return cc[c];
}

q_t scan_char(q_t q, int c)
{
    return delta[q][cc[c]];
}

main()
{
    q_t q = 0;
    int c;
    while ((c = getchar()) != EOF) {
	q = scan_char(q, c);
    }
}
