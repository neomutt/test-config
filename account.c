#include <stddef.h>
#include "account.h"
#include "lib/lib.h"
#include "config_set.h"
#include "mutt_options.h"
#include "inheritance.h"

struct Account *ac_create(struct ConfigSet *cs, const char *name, const char *var_names[])
{
  if (!cs || !name || !var_names)
    return NULL;

  int count = 0;
  for (; var_names[count]; count++)
    ;

  struct Account *ac = safe_calloc(1, sizeof(*ac));
  ac->name = safe_strdup(name);
  ac->cs = cs;
  ac->var_names = var_names;
  ac->vars = safe_calloc(count, sizeof(struct HashElem *));
  ac->num_vars = count;

  bool success = true;
  char acname[64];

  for (int i = 0; i < ac->num_vars; i++)
  {
    struct HashElem *parent = cs_get_elem(cs, ac->var_names[i]);
    if (!parent)
    {
      printf("%s doesn't exist\n", ac->var_names[i]);
      success = false;
      break;
    }

    snprintf(acname, sizeof(acname), "%s:%s", name, ac->var_names[i]);
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

  ac_free(cs, &ac);
  return NULL;
}

void ac_free(struct ConfigSet *cs, struct Account **ac)
{
  if (!ac)
    return;

  char child[128];

  for (int i = 0; i < (*ac)->num_vars; i++)
  {
    snprintf(child, sizeof(child), "%s:%s", (*ac)->name, (*ac)->var_names[i]);
    cs_reset_variable(cs, child, NULL);
  }

  FREE(&(*ac)->name);
  FREE(&(*ac)->vars);
  FREE(ac);
}

bool ac_set_value(const struct Account *ac, int vid, intptr_t value, struct Buffer *err)
{
  if (!ac)
    return false;
  if ((vid < 0) || (vid >= ac->num_vars))
    return false;

  struct HashElem *he = ac->vars[vid];
  return cs_he_set_value(ac->cs, he, value, err);
}

bool ac_get_value(const struct Account *ac, int vid, struct Buffer *err)
{
  if (!ac)
    return false;
  if ((vid < 0) || (vid >= ac->num_vars))
    return false;

  struct HashElem *he = ac->vars[vid];

  if ((he->type & DT_INHERITED) && (DTYPE(he->type) == 0))
  {
    struct Inheritance *i = he->data;
    he = i->parent;
  }

  return cs_he_get_value(ac->cs, he, err);
}

