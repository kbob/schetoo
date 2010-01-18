#ifndef OBJ_SYMBOL_INCLUDED
#define OBJ_SYMBOL_INCLUDED

#include "obj.h"

OBJ_TYPE_PREDICATE(symbol)		// bool is_symbol(obj_t);

extern obj_t make_symbol           (obj_t name);
extern obj_t make_symbol_from_C_str(const wchar_t *name);
extern obj_t make_anonymous_symbol (void);
extern obj_t symbol_name           (obj_t);

#endif /* !OBJ_SYMBOL_INCLUDED */
