#ifndef MEM_SCALAR_INCLUDED
#define MEM_SCALAR_INCLUDED

#include "mem.h"

/*
 * A scalar is an abstract base type for data elements that do not
 * contain object pointers.
 * 
 * Example scalars: string, bytevector
 */

extern void mem_scalar_create_ops(mem_ops_t *, wchar_t *name, mem_size_op);

#endif /* !MEM_SCALAR_INCLUDED */
