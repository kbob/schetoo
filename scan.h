#ifndef SCAN_INCLUDED
#define SCAN_INCLUDED

#include "cv.h"
#include "io.h"
#include "obj.h"

typedef enum token_type {
    TOK_U8,
    TOK_SIMPLE,
    TOK_ABBREV,
    TOK_COMMENT,
    TOK_BEGIN_VECTOR,
    TOK_BEGIN_BYTEVECTOR,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_PERIOD,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_EOF,

    TOKEN_TYPE_COUNT
} token_type_t;

extern cv_t         c_read_token(obj_t cont, obj_t values);
extern token_type_t yylex       (obj_t *lvalp, instream_t *in);
extern const char  *token_name  (token_type_t tok);

#endif /* !SCAN_INCLUDED */
