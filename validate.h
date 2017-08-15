#ifndef _MUTT_VALIDATE_H
#define _MUTT_VALIDATE_H

#include <stdbool.h>
#include <stdint.h>

struct ConfigSet;
struct VariableDef;
struct Buffer;

bool val_path(const struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_str(const struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_addr(const struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_magic(const struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_num(const struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_quad(const struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_rx(const struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_bool(const struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_sort(const struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_mbchartbl(const struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);

#endif /* _MUTT_VALIDATE_H */
