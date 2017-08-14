#include <stddef.h>
#include "account.h"
#include "lib/lib.h"
#include "config_set.h"

const char *AccountVarStr[] = {
  "alias_file",           /* DT_PATH */
  "attribution",          /* DT_STR */
  "from",                 /* DT_ADDR */
  "header_cache_backend", /* DT_HCACHE */
  "mbox_type",            /* DT_MAGIC */
  "pager_context",        /* DT_NUM */
  "post_moderated",       /* DT_QUAD */
  "quote_regexp",         /* DT_RX */
  "resume_draft_files",   /* DT_BOOL */
  "sort",                 /* DT_SORT */
  "status_chars",         /* DT_MBCHARTBL */
};

struct Account *account_create(const char *name, struct ConfigSet *cs)
{
  if (!name || !cs)
    return NULL;

  struct Account *ac = safe_calloc(1, sizeof(*ac));
  bool success = true;
  char acname[64];

  for (int i = 0; i < mutt_array_size(AccountVarStr); i++)
  {
    struct HashElem *parent = cs_get_elem(cs, AccountVarStr[i]);
    if (!parent)
    {
      printf("%s doesn't exist\n", AccountVarStr[i]);
      success = false;
      break;
    }

    snprintf(acname, sizeof(acname), "%s:%s", name, AccountVarStr[i]);
    ac->vars[i] = cs_inherit_variable(cs, parent, acname);
    if (!ac->vars[i])
    {
      printf("failed to create %s\n", acname);
      success = false;
      break;
    }
  }

  if (success)
    return ac;

  account_free(&ac);
  return NULL;
}

void account_free(struct Account **ac)
{
  FREE(ac);
}

