#ifndef _MUTT_ACCOUNT_H
#define _MUTT_ACCOUNT_H

#include <stdbool.h>
#include <stdint.h>

struct Buffer;
struct ConfigSet;
struct HashElem;

struct Account
{
  char *name;
  struct ConfigSet *cs;
  const char **var_names;
  int num_vars;
  struct HashElem **vars;
};

struct Account *ac_create(struct ConfigSet *cs, const char *name, const char *var_names[]);
void ac_free(struct ConfigSet *cs, struct Account **ac);

bool ac_set_value(const struct Account *ac, int vid, intptr_t value, struct Buffer *err);
bool ac_get_value(const struct Account *ac, int vid, struct Buffer *err);

#endif /* _MUTT_ACCOUNT_H */
