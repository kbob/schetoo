#include "obj_port.h"

mem_ops_t port_ops;

obj_t make_port(port_flags_t flags,
		int          fd,
		obj_t        read_proc,
		obj_t        write_proc,
		obj_t        seek_proc,
		obj_t        close_proc,
		obj_t        read_buffer,
		obj_t        write_buffer)
{
    word_t flfd = flags | fd << PORT_FD_SHIFT;
    if (!port_ops.mo_super)
	mem_mixvec_create_ops(&port_ops,
			      L"port", PORT_SCALAR_COUNT, PORT_OBJ_COUNT);
    return PORT_MV_MAKE(&port_ops,
			flfd, 0, 0, 0, 0,
			read_proc, write_proc, seek_proc, close_proc,
			read_buffer, write_buffer);
}
