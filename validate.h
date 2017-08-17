#ifndef _MUTT_VALIDATE_H
#define _MUTT_VALIDATE_H

#include <stdbool.h>
#include <stdint.h>

struct ConfigSet;
struct VariableDef;
struct Buffer;

bool val_path(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_str(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_addr(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_magic(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_num(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_quad(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_rx(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_bool(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_sort(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
bool val_mbchartbl(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);

#endif /* _MUTT_VALIDATE_H */
