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
 * config_subset_free - XXX
 * @param sub XXX
 */
void config_subset_free(struct ConfigSubset **sub)
{
  if (!sub || !*sub)
    return;

  FREE(&(*sub)->name);
  FREE(&(*sub)->vars);
  FREE(sub);
}

/**
 * config_subset_new - XXX
 */
struct ConfigSubset *config_subset_new(const struct ConfigSet *cs,
                                       const char *name, const char *parent_name, const char *var_names[])
{
  if (!cs || !name || !var_names)
    return NULL;

  size_t count = 0;
  for (; var_names[count]; count++)
    ;

  struct ConfigSubset *sub = mutt_mem_calloc(1, sizeof(*sub));
  sub->name = mutt_str_strdup(name);
  sub->cs = cs;
  sub->var_names = var_names;
  sub->vars = mutt_mem_calloc(count, sizeof(struct HashElem *));
  sub->num_vars = count;

  char sub_name[128];

  for (size_t i = 0; i < sub->num_vars; i++)
  {
    if (parent_name)
    {
      snprintf(sub_name, sizeof(sub_name), "%s:%s", parent_name, sub->var_names[i]);
    }
    else
    {
      mutt_str_strfcpy(sub_name, sub->var_names[i], sizeof(sub_name));
    }

    struct HashElem *parent = cs_get_elem(cs, sub_name);
    if (!parent)
    {
      mutt_debug(LL_DEBUG1, "%s doesn't exist\n", sub_name);
      config_subset_free(&sub);
      return NULL;
    }

    snprintf(sub_name, sizeof(sub_name), "%s:%s", name, sub->var_names[i]);
    sub->vars[i] = cs_inherit_variable(cs, parent, sub_name);
    if (!sub->vars[i])
    {
      mutt_debug(LL_DEBUG1, "failed to create %s\n", sub_name);
      config_subset_free(&sub);
      return NULL;
    }
  }

  return sub;
}

