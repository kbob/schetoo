#ifndef OBJ_PORT_INCLUDED
#define OBJ_PORT_INCLUDED

#include <sys/types.h>

#include "mem_mixvec.h"
#include "obj.h"
#include "uniq.h"

typedef enum port_flags {
    PORT_TEXTUAL  = 1 << 0,
    PORT_INPUT    = 1 << 1,
    PORT_OUTPUT   = 1 << 2,
    PORT_SEEKABLE = 1 << 3,
    PORT_FD_SHIFT = 4
} port_flags_t;

typedef enum port_scalars {
    PS_FLFD,
    PS_RPOS,
    PS_WPOS,
    PS_ROFF,
    PS_WOFF,
} port_scalars_t;
#define PORT_SCALAR_COUNT 5

typedef enum port_objects {
    PO_READ,
    PO_WRITE,
    PO_SEEK,
    PO_CLOSE,
    PO_RBUF,
    PO_WBUF,
} port_objects_t;
#define PORT_OBJ_COUNT 6

#define PORT_MV CAT_(mixvec_, CAT_(PORT_SCALAR_COUNT, CAT_(_, PORT_OBJ_COUNT)))
#define PORT_MV_T       CAT_(PORT_MV, _t)
#define PORT_MV_MAKE    CAT_(make_, PORT_MV)
#define PORT_MV_GET_INT CAT_(PORT_MV, _get_int)
#define PORT_MV_SET_INT CAT_(PORT_MV, _set_int)
#define PORT_MV_GET_PTR CAT_(PORT_MV, _get_ptr)
#define PORT_MV_SET_PTR CAT_(PORT_MV, _set_ptr)

typedef PORT_MV_T port_mixvec_t;

extern mem_ops_t port_ops;

extern obj_t  make_port               (port_flags_t flags,
                                       int          fd,
                                       obj_t        read_proc,
                                       obj_t        write_proc,
                                       obj_t        seek_proc,
                                       obj_t        close_proc,
                                       obj_t        read_buffer,
                                       obj_t        write_buffer);

static inline bool   port_is_textual  (obj_t port);
static inline bool   port_is_binary   (obj_t port);
static inline bool   port_is_input    (obj_t port);
static inline bool   port_is_output   (obj_t port);
static inline bool   port_is_inout    (obj_t port);
static inline bool   port_is_seekable (obj_t port);

static inline bool   is_port          (obj_t obj);
static inline bool   is_textual_port  (obj_t port);
static inline bool   is_binary_port   (obj_t port);
static inline bool   is_input_port    (obj_t port);
static inline bool   is_output_port   (obj_t port);
static inline bool   is_inout_port    (obj_t port);
static inline bool   is_seekable_port (obj_t port);

static inline int    port_fd          (obj_t port);
static inline size_t port_read_pos    (obj_t port);
static inline size_t port_write_pos   (obj_t port);
static inline off_t  port_read_offset (obj_t port);
static inline off_t  port_write_offset(obj_t port);
static inline obj_t  port_read_proc   (obj_t port);
static inline obj_t  port_write_proc  (obj_t port);
static inline obj_t  port_seek_proc   (obj_t port);
static inline obj_t  port_close_proc  (obj_t port);
static inline obj_t  port_read_buffer (obj_t port);
static inline obj_t  port_write_buffer(obj_t port);

OBJ_TYPE_PREDICATE(port);

static inline bool port_is_textual(obj_t port)
{
    return (PORT_MV_GET_INT(port, PS_FLFD) & PORT_TEXTUAL) ? true : false;
}

static inline bool port_is_binary(obj_t port)
{
    return !port_is_textual(port);
}

static inline bool port_is_input(obj_t port)
{
    return (PORT_MV_GET_INT(port, PS_FLFD) & PORT_INPUT) ? true : false;
}

static inline bool port_is_output(obj_t port)
{
    return (PORT_MV_GET_INT(port, PS_FLFD) & PORT_OUTPUT) ? true : false;
}

static inline bool port_is_inout(obj_t port)
{
    return port_is_input(port) && port_is_output(port);
}

static inline bool port_is_seekable(obj_t port)
{
    return (PORT_MV_GET_INT(port, PS_FLFD) & PORT_SEEKABLE) ? true : false;
}

static inline bool is_textual_port(obj_t port)
{
    return is_port(port) && port_is_textual(port);
}

static inline bool is_binary_port(obj_t port)
{
    return is_port(port) && port_is_binary(port);
}

static inline bool is_input_port(obj_t port)
{
    return is_port(port) && port_is_input(port);
}

static inline bool is_output_port(obj_t port)
{
    return is_port(port) && port_is_output(port);
}

static inline bool is_inout_port(obj_t port)
{
    return is_port(port) && port_is_inout(port);
}

static inline bool is_seekable_port(obj_t port)
{
    return is_port(port) && port_is_seekable(port);
}

static inline int port_fd(obj_t port)
{
    return PORT_MV_GET_INT(port, PS_FLFD) >> PORT_FD_SHIFT;
}

static inline size_t port_read_pos(obj_t port)
{
    return PORT_MV_GET_INT(port, PS_RPOS);
}

static inline size_t port_write_pos(obj_t port)
{
    return PORT_MV_GET_INT(port, PS_WPOS);
}

static inline off_t port_read_offset(obj_t port)
{
    return PORT_MV_GET_INT(port, PS_ROFF);
}

static inline off_t port_write_offset(obj_t port)
{
    return PORT_MV_GET_INT(port, PS_WOFF);
}

static inline obj_t port_read_proc(obj_t port)
{
    return PORT_MV_GET_PTR(port, PO_READ);
}

static inline obj_t port_write_proc(obj_t port)
{
    return PORT_MV_GET_PTR(port, PO_WRITE);
}

static inline obj_t port_seek_proc(obj_t port)
{
    return PORT_MV_GET_PTR(port, PO_SEEK);
}

static inline obj_t port_close_proc(obj_t port)
{
    return PORT_MV_GET_PTR(port, PO_CLOSE);
}

static inline obj_t port_read_buffer(obj_t port)
{
    return PORT_MV_GET_PTR(port, PO_RBUF);
}

static inline obj_t port_write_buffer(obj_t port)
{
    return PORT_MV_GET_PTR(port, PO_WBUF);
}

#endif /* !OBJ_PORT_INCLUDED */
