#include "conditions.h"

#include <assert.h>
#include <stdarg.h>

#include "obj_boolean.h"
#include "obj_record.h"
#include "obj_vector.h"
#include "oprintf.h"
#include "record.h"

/* standard simple conditions */

DEFINE_EXTERN_RECORD_TYPE(condition, L"&condition", NULL,       0) = {
    { FM_END }
};

DEFINE_EXTERN_RECORD_TYPE(message,   L"&message",   &condition, 0) = {
    { FM_IMMUTABLE, L"message" },
    { FM_END }
};

DEFINE_EXTERN_RECORD_TYPE(warning,   L"&warning",   &condition, 0) = {
    { FM_END }
};

DEFINE_EXTERN_RECORD_TYPE(serious,   L"&serious",   &condition, 0) = {
    { FM_END }
};

DEFINE_EXTERN_RECORD_TYPE(error,     L"&error",     &serious,   0) = {
    { FM_END }
};

DEFINE_EXTERN_RECORD_TYPE(violation, L"&violation", &serious,   0) = {
    { FM_END }
};

DEFINE_EXTERN_RECORD_TYPE(assertion, L"&assertion", &serious,   0) = {
    { FM_END }
};

DEFINE_EXTERN_RECORD_TYPE(irritants, L"&irritants", &condition, 0) = {
    { FM_IMMUTABLE, L"irritants" },
    { FM_END }
};

DEFINE_EXTERN_RECORD_TYPE(who,       L"&who",       &condition, 0) = {
    { FM_IMMUTABLE, L"who" },
    { FM_END }
};

DEFINE_EXTERN_RECORD_TYPE(non_continuable,
			  L"&non_continuable",
			  &violation,
			  0) = {
    { FM_END }
};

DEFINE_EXTERN_RECORD_TYPE(implementation_restriction,
			  L"&implementation_restriction",
			  &violation, 0) = {
    { FM_END }
};

DEFINE_EXTERN_RECORD_TYPE(lexical,   L"&lexical",   &violation, 0) = {
    { FM_END },
};

DEFINE_EXTERN_RECORD_TYPE(syntax,    L"&syntax",    &violation, 0) = {
    { FM_IMMUTABLE, L"form" },
    { FM_IMMUTABLE, L"subform" },
    { FM_END },
};

DEFINE_EXTERN_RECORD_TYPE(undefined, L"&undefined", &violation, 0) = {
    { FM_END }
};

/* compound conditions */

DEFINE_STATIC_RECORD_TYPE(compound_condition,
			  L"&compound-condition",
			  NULL,
			  RF_SEALED | RF_OPAQUE) = {
    { FM_MUTABLE, L"parts" },
    { FM_END }
};

obj_t make_compound_condition_(obj_t parts, ...)
{
    size_t i, j, n_parts = 1;
    obj_t p;
    va_list ap;

    va_start(ap, parts);
    while ((p = va_arg(ap, obj_t)) != END_OF_ARGS) {
	if (is_instance(p, condition))
	    n_parts++;
	else if (is_instance(p, compound_condition))
	    n_parts += vector_len(record_get_field(p, 0));
	else
	    CHECK(false, "must be condition", p);
    }
    va_end(ap);

    obj_t vec = make_vector_uninitialized(n_parts);
    i = 0;
    vector_set(vec, i++, parts);
    va_start(ap, parts);
    while ((p = va_arg(ap, obj_t)) != END_OF_ARGS) {
	if (is_instance(p, condition))
	    vector_set(vec, i++, p);
	else if (is_instance(p, compound_condition)) {
	    obj_t part_parts = record_get_field(p, 0);
	    for (j = 0; j < vector_len(part_parts); j++)
		vector_set(vec, i++, vector_ref(part_parts, j));
	} else
	    assert(false);
    }
    assert(i == n_parts);
    va_end(ap);
    return MAKE_RECORD(compound_condition, vec);
}
