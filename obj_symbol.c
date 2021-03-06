#include "obj_symbol.h"

#include <assert.h>
#include <unistd.h>
#include <wchar.h>

#include "except.h"
#include "mem_fixvec.h"
#include "obj_null.h"
#include "obj_pair.h"
#include "obj_string.h"
#include "obj_uninit.h"
#include "roots.h"

mem_ops_t symbol_ops;
static int gen_name_counter;

ROOT_CONSTRUCTOR(all_symbols_list)
{
    return make_null();
}

static inline obj_t alloc_symbol(obj_t name)
{
    if (!symbol_ops.mo_super)
	mem_fixvec_create_ops(&symbol_ops, L"symbol", 1);
    return make_fixvec1(&symbol_ops, name);
}

static obj_t find_symbol(obj_t name)
{
    obj_t p, sym;
    obj_t sym_name;

    CHECK(is_string(name), "must be string", name);
    for (p = all_symbols_list; !is_null(p); p = pair_cdr(p)) {
	assert(is_pair(p));
	sym = pair_car(p);
	assert(is_symbol(sym));
	sym_name = symbol_name(sym);
	assert(is_string(sym_name));
	if (strings_are_equal(sym_name, name))
	    return sym;
    }
    return EMPTY_LIST;
}

obj_t make_symbol(obj_t name)
{
    obj_t symbol = find_symbol(name);
    if (is_null(symbol)) {
	/* Not found.  Create one. */
	symbol = alloc_symbol(name);
	/* with lock */ {
	    /* verify symbol still absent. */
	    all_symbols_list = make_pair(symbol, all_symbols_list);
	}
    }
    return symbol;
}

obj_t make_symbol_from_chars(const wchar_t *C_name, size_t len)
{
    obj_t name = make_string_from_chars(C_name, len);
    return make_symbol(name);
}

obj_t make_symbol_from_C_str(const wchar_t *C_name)
{
    return make_symbol_from_chars(C_name, wcslen(C_name));
}

obj_t make_anonymous_symbol(void)
{
    return alloc_symbol(make_uninitialized());
}

obj_t symbol_name(obj_t symbol)
{
    CHECK_OBJ(symbol);
    CHECK(is_symbol(symbol), "must be symbol", symbol);
    obj_t name = fixvec1_get_ptr(symbol, 0);
    if (is_uninitialized(name)) {
	size_t max_len = 12;
	ssize_t name_len;
	wchar_t name_buf[max_len];
	while (true) {
	    name_len = swprintf(name_buf, max_len,
				L"g%04d", ++gen_name_counter);
	    assert(0 <= name_len && name_len < max_len);
	    name = make_string_from_chars(name_buf, name_len);
	    if (!is_null(find_symbol(name)))
		continue;
	    /* with lock */ {
		/* verify symbol still absent */
		fixvec1_set_ptr(symbol, 0, name);
		all_symbols_list = make_pair(symbol, all_symbols_list);
	    }
	    break;
	}
    }
    return name;
}
