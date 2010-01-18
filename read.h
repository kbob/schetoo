#ifndef READ_INCLUDED
#define READ_INCLUDED

#include "io.h"
#include "mem.h"

/* Returns true if expression read, false at EOF. */
extern bool read_stream(instream_t *in, obj_t *obj_out);

#endif /* !READ_INCLUDED */
