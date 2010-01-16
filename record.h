#define DECLARE_RECORD_TYPE(name, parent, uid, is_sealed, is_opaque, fields...) \
bool is_##name(obj_t obj) { ... } \
obj_t *make_##name(obj_t, ...) { ... }

