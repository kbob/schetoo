#ifndef IO_INCLUDED
#define IO_INCLUDED

#include <setjmp.h>			/* for jmp_buf */
#include <stddef.h>			/* for wchar_t */
#include <stdio.h>			/* for FILE    */
#include <sys/types.h>			/* for off_t   */
#include <wchar.h>			/* for wint_t  */

#include "char.h"

typedef struct instream instream_t;
typedef struct outstream outstream_t;

extern instream_t  *make_file_instream    (FILE *);
extern instream_t  *make_readline_instream(void);
extern instream_t  *make_string_instream  (const wchar_t *, size_t);
extern void         delete_instream       (instream_t *);
extern wint_t       instream_getwc        (instream_t *);
extern wint_t       instream_ungetwc      (wint_t, instream_t *);

extern outstream_t *make_file_outstream  (FILE *);
extern outstream_t *make_string_outstream(wchar_t *, size_t);
extern void         delete_outstream     (outstream_t *);
extern wint_t       outstream_putwc      (wchar_t, outstream_t *);
extern int          outstream_printf     (outstream_t *,
                                          const wchar_t *fmt, ...);
extern off_t        outstream_pos        (outstream_t *);
extern jmp_buf     *outstream_set_limit  (outstream_t *, off_t limit);
extern void         outstream_clear_limit(outstream_t *);

#endif /* !IO_INCLUDED */
