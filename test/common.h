#ifndef _TEST_COMMON_H
#define _TEST_COMMON_H

#include <stdbool.h>
#include <stdint.h>

struct ConfigSet;
struct VariableDef;
struct Buffer;

extern const char *line;

bool validator_fail(const struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *result);
bool validator_succeed(const struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *result);
void log_line(const char *fn);
bool log_listener(const struct ConfigSet *cs, struct HashElem *he, const char *name, enum ConfigEvent ev);
void set_list(const struct ConfigSet *cs);

#endif /* _TEST_COMMON_H */
