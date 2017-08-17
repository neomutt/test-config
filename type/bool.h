#ifndef _MUTT_BOOL_H
#define _MUTT_BOOL_H

void init_bool(void);

bool set_he_bool_err(struct Account *ac, int vid, bool value, struct Buffer *err);
bool set_he_bool(struct Account *ac, int vid, bool value);

bool get_he_bool_err(struct Account *ac, int vid, struct Buffer *err);
bool get_he_bool(struct Account *ac, int vid);

bool     set_bool_native(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
intptr_t get_bool_native(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, struct Buffer *err);

#endif /* _MUTT_BOOL_H */

