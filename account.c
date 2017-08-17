#include <stddef.h>
#include "account.h"
#include "lib/lib.h"
#include "config_set.h"
#include "mutt_options.h"
#include "inheritance.h"

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
  NULL,
};

struct Account *account_create(struct ConfigSet *cs, const char *name)
{
  if (!cs || !name)
    return NULL;

  struct Account *ac = safe_calloc(1, sizeof(*ac));
  ac->name = safe_strdup(name);
  ac->cs = cs;

  bool success = true;
  char acname[64];

  for (int i = 0; AccountVarStr[i]; i++)
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

  account_free(cs, &ac);
  return NULL;
}

void account_free(struct ConfigSet *cs, struct Account **ac)
{
  if (!ac)
    return;

  char child[128];

  for (int i = 0; i < V_MAX; i++)
  {
    snprintf(child, sizeof(child), "%s:%s", (*ac)->name, AccountVarStr[i]);
    cs_reset_variable(cs, child, NULL);
  }

  FREE(&(*ac)->name);
  FREE(ac);
}

bool account_set_value(const struct Account *ac, int vid, intptr_t value, struct Buffer *err)
{
  struct HashElem *he = ac->vars[vid];
  return cs_set_value(ac->cs, he, value, err);
}

bool account_get_value(const struct Account *ac, int vid, struct Buffer *err)
{
  struct HashElem *he = ac->vars[vid];

  if ((he->type & DT_INHERITED) && (DTYPE(he->type) == 0))
  {
    struct Inheritance *i = he->data;
    he = i->parent;
  }

  return cs_get_value(ac->cs, he, err);
}

