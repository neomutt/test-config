/**
 * @file
 * Subset of Config Items
 *
 * @authors
 * Copyright (C) 2017-2018 Richard Russon <rich@flatcap.org>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @page config_subset Subset of Config Items
 *
 * Subset of Config Items
 */

#include "config.h"
#include <stddef.h>
#include <limits.h>
#include <stdint.h>
#include "mutt/mutt.h"
#include "subset.h"
#include "set.h"
#include "types.h"

/**
 * cs_subset_free - XXX
 * @param sub XXX
 */
void cs_subset_free(struct ConfigSubset **ptr)
{
  if (!ptr || !*ptr)
    return;

  struct ConfigSubset *sub = *ptr;

  char scope[128];
  for (size_t i = 0; i < sub->num_vars; i++)
  {
    if (sub->scope)
      snprintf(scope, sizeof(scope), "%s:%s", sub->scope, sub->var_names[i]);
    else
      mutt_str_strfcpy(scope, sub->var_names[i], sizeof(scope));

    printf("uninherit %s\n", scope);
    cs_uninherit_variable(sub->cs, scope);
  }

  // sub->var_names isn't ours to free
  FREE(&sub->scope);
  FREE(&sub->vars);
  FREE(ptr);
}

/**
 * cs_subset_new - XXX
 */
struct ConfigSubset *cs_subset_new(const struct ConfigSet *cs,
                                       const char *name, const char *parent_name, const char *var_names[])
{
  if (!cs || !name || !var_names)
    return NULL;

  size_t count = 0;
  for (; var_names[count]; count++)
    ;

  char scope[128];
  if (parent_name)
    snprintf(scope, sizeof(scope), "%s:%s", parent_name, name);
  else
    mutt_str_strfcpy(scope, name, sizeof(scope));

  struct ConfigSubset *sub = mutt_mem_calloc(1, sizeof(*sub));
  sub->scope = mutt_str_strdup(scope);
  sub->cs = cs;
  sub->var_names = var_names;
  sub->vars = mutt_mem_calloc(count, sizeof(struct HashElem *));
  sub->num_vars = count;

  char tmp[130];
  for (size_t i = 0; i < sub->num_vars; i++)
  {
    if (parent_name)
    {
      snprintf(tmp, sizeof(tmp), "%s:%s", parent_name, sub->var_names[i]);
    }
    else
    {
      mutt_str_strfcpy(tmp, sub->var_names[i], sizeof(tmp));
    }

    struct HashElem *parent = cs_get_elem(cs, tmp);
    if (!parent)
    {
      mutt_debug(LL_DEBUG1, "%s doesn't exist\n", tmp);
      cs_subset_free(&sub);
      return NULL;
    }

    snprintf(tmp, sizeof(tmp), "%s:%s", scope, sub->var_names[i]);
    sub->vars[i] = cs_inherit_variable(cs, parent, tmp);
    if (!sub->vars[i])
    {
      mutt_debug(LL_DEBUG1, "failed to create %s\n", tmp);
      cs_subset_free(&sub);
      return NULL;
    }
  }

  return sub;
}

/**
 * cs_subset_lookup - XXX
 */
int cs_subset_lookup(struct ConfigSubset *sub, const char *name)
{
  if (!sub || !name)
    return -1;

  return -1;
}

/**
 * cs_subset_native_get - XXX
 */
intptr_t cs_subset_native_get(const struct ConfigSubset *sub, int vid, struct Buffer *err)
{
  if (!sub || (vid < 0) || (vid >= sub->num_vars))
    return INT_MIN;

  return INT_MIN;
}

/**
 * cs_subset_native_set - XXX
 */
int cs_subset_native_set(const struct ConfigSubset *sub, int vid, intptr_t value, struct Buffer *err)
{
  if (!sub || (vid < 0) || (vid >= sub->num_vars))
    return CSR_ERR_CODE;

  return CSR_ERR_INVALID;
}

/**
 * cs_subset_reset - XXX
 */
int cs_subset_reset(const struct ConfigSubset *sub, int vid, struct Buffer *err)
{
  if (!sub || (vid < 0) || (vid >= sub->num_vars))
    return CSR_ERR_CODE;

  struct HashElem *he = sub->vars[vid];

  return cs_he_reset(sub->cs, he, err);
}

/**
 * cs_subset_string_get - XXX
 */
int cs_subset_string_get(const struct ConfigSubset *sub, int vid, struct Buffer *result)
{
  if (!sub || (vid < 0) || (vid >= sub->num_vars))
    return CSR_ERR_CODE;

  struct HashElem *he = sub->vars[vid];

  return cs_he_string_get(sub->cs, he, result);
}

/**
 * cs_subset_string_set - XXX
 */
int cs_subset_string_set(const struct ConfigSubset *sub, int vid, const char *value, struct Buffer *err)
{
  if (!sub || (vid < 0) || (vid >= sub->num_vars))
    return INT_MIN;

  struct HashElem *he = sub->vars[vid];

  return cs_he_string_set(sub->cs, he, value, err);
}
