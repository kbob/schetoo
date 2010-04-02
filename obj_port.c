#include "obj_port.h"

mem_ops_t port_ops;

obj_t make_port(port_flags_t flags,
		int          fd,
		obj_t        read_proc,
		obj_t        write_proc,
		obj_t        seek_proc,
		obj_t        close_proc,
		obj_t        text_buffer,
		obj_t        binary_buffer)
{
    CHECK_OBJ(read_proc);
    CHECK_OBJ(write_proc);
    CHECK_OBJ(seek_proc);
    CHECK_OBJ(close_proc);
    CHECK_OBJ(text_buffer);
    CHECK_OBJ(binary_buffer);
    word_t flfd = flags | fd << PORT_FD_SHIFT;
    if (!port_ops.mo_super)
	mem_mixvec_create_ops(&port_ops,
			      L"port", PORT_SCALAR_COUNT, PORT_OBJ_COUNT);
    return PORT_MV_MAKE(&port_ops,
			flfd, 0, 0, 0, 0,
			read_proc, write_proc, seek_proc, close_proc,
			text_buffer, binary_buffer);
}

void port_set_text_pos(obj_t port, size_t pos)
{
    PORT_MV_SET_INT(port, PS_TPOS, pos);
}

void port_set_binary_pos(obj_t port, size_t pos)
{
    PORT_MV_SET_INT(port, PS_BPOS, pos);
}

void port_set_binary_len(obj_t port, size_t len)
{
    PORT_MV_SET_INT(port, PS_BLEN, len);
}

void port_set_buffer_offset(obj_t port, off_t offset)
{
    PORT_MV_SET_INT(port, PS_BOFF, offset);
}

void port_set_text_buffer(obj_t port, obj_t  buffer)
{
    PORT_MV_SET_PTR(port, PO_TBUF, buffer);
}
