#ifndef READ_INCLUDED
#define READ_INCLUDED

#include "cv.h"
#include "io.h"
#include "obj.h"

/* Returns true if expression read, false at EOF. */
extern bool read_stream(instream_t *in, obj_t *obj_out);

extern cv_t c_parse(obj_t cont, obj_t values);

#endif /* !READ_INCLUDED */
