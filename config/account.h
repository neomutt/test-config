#ifndef _CONFIG_ACCOUNT_H
#define _CONFIG_ACCOUNT_H

#include <stdint.h>

struct Buffer;
struct ConfigSet;

struct Account
{
  char *name;
  const struct ConfigSet *cs;
  const char **var_names;
  int num_vars;
  struct HashElem **vars;
};

struct Account *ac_create(const struct ConfigSet *cs, const char *name, const char *var_names[]);
void ac_free(const struct ConfigSet *cs, struct Account **ac);

int ac_set_value(const struct Account *ac, unsigned int vid, intptr_t value, struct Buffer *err);
int ac_get_value(const struct Account *ac, unsigned int vid, struct Buffer *err);

#endif /* _CONFIG_ACCOUNT_H */
