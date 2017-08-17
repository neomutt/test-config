#ifndef _MUTT_ACCOUNT_H
#define _MUTT_ACCOUNT_H

#include <stdbool.h>
#include <stdint.h>

struct Buffer;
struct ConfigSet;
struct HashElem;

extern const char *AccountVarStr[];

enum AccountVar
{
  V_ALIAS_FILE,
  V_ATTRIBUTION,
  V_FROM,
  V_HEADER_CACHE_BACKEND,
  V_MBOX_TYPE,
  V_PAGER_CONTEXT,
  V_POST_MODERATED,
  V_QUOTE_REGEXP,
  V_RESUME_DRAFT_FILES,
  V_SORT,
  V_STATUS_CHARS,
  V_MAX,
};

struct Account
{
  char *name;
  struct ConfigSet *cs;
  struct HashElem *vars[V_MAX];
};

struct Account *account_create(struct ConfigSet *cs, const char *name);
void account_free(struct ConfigSet *cs, struct Account **ac);

bool account_set_value(const struct Account *ac, int vid, intptr_t value, struct Buffer *err);
bool account_get_value(const struct Account *ac, int vid, struct Buffer *err);

#endif /* _MUTT_ACCOUNT_H */
