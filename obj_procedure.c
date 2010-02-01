#include "obj_procedure.h"

static size_t proc_size_op(const heap_object_t *hobj)
{
    return sizeof (proc_obj_t);
}

static size_t proc_ptr_count_op(const heap_object_t *hobj)
{
    return ((proc_obj_t *)hobj)->proc_flags & PF_COMPILED_C ? 2 : 3;
}

static void proc_move_op(const heap_object_t *src, heap_object_t *dst)
{
    *(proc_obj_t *)dst = *(const proc_obj_t *)src;
}

#include <stdio.h>

static obj_t proc_get_ptr_op(const heap_object_t *hobj, size_t index)
{
    proc_obj_t *proc = (proc_obj_t *)hobj;
    if (index == 0)
	return proc->proc_args;
    if (index == 1)
	return proc->proc_env;
    if (index == 2 && !(proc->proc_flags & PF_COMPILED_C))
	return proc->proc_u.pu_body;
    ASSERT(false && "index out of range");
}

static void proc_set_ptr_op(heap_object_t *hobj, size_t index, obj_t ptr)
{
    proc_obj_t *proc = (proc_obj_t *)hobj;
    if (index == 0)
	proc->proc_args = ptr;
    else if (index == 1)
	proc->proc_env = ptr;
    else if (index == 2 && !(proc->proc_flags & PF_COMPILED_C))
	proc->proc_u.pu_body = ptr;
    else
	ASSERT(false && "index out of range");
}

mem_ops_t procedure_ops = {
    MEM_OPS_PRIMITIVE,
    L"procedure",
    NULL,
    proc_size_op,
    proc_ptr_count_op,
    proc_move_op,
    proc_get_ptr_op,
    proc_set_ptr_op,
    { }
};

static obj_t make_proc(int flags, obj_t body, obj_t args, obj_t env)
{
    heap_object_t *obj = mem_alloc_obj(&procedure_ops, sizeof (proc_obj_t));
    proc_obj_t *proc = (proc_obj_t *)obj;
    proc->proc_flags = flags;
    proc->proc_args = args;
    proc->proc_env = env;
    proc->proc_u.pu_body = body;
    return (obj_t)obj;
}

obj_t make_procedure(obj_t body, obj_t arglist, obj_t env)
{
    CHECK_OBJ(body);
    CHECK_OBJ(arglist);
    CHECK_OBJ(env);
    return make_proc(0, body, arglist, env);
}

obj_t make_special_form_procedure(obj_t body, obj_t env)
{
    CHECK_OBJ(body);
    CHECK_OBJ(env);
    return make_proc(PF_SPECIAL_FORM, body, UNDEFINED_OBJ, env);
}

obj_t make_C_procedure(C_procedure_t code, interval_t arg_range, obj_t env)
{
    return make_proc(PF_COMPILED_C, (obj_t)code, make_fixnum(arg_range), env);
}

obj_t make_C_special_form_procedure(C_procedure_t code,
				    obj_t         env)
{
    return make_proc(PF_COMPILED_C | PF_SPECIAL_FORM, (obj_t)code, UNDEFINED_OBJ, env);
}
