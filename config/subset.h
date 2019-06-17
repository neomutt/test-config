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

#ifndef MUTT_CONFIG_SUBSET_H
#define MUTT_CONFIG_SUBSET_H

#include <stdio.h>
#include "set.h"

/**
 * struct ConfigSubset - XXX
 */
struct ConfigSubset
{
  char *scope;                ///< Scope name of Subset
  const struct ConfigSet *cs; ///< Parent ConfigSet
  size_t num_vars;            ///< Number of local config items
  const char **var_names;     ///< Array of the names of local config items
  struct HashElem **vars;     ///< Array of the HashElems of Subset config items
};

struct ConfigSubset *cs_subset_new(const struct ConfigSet *cs, const char *name, const char *parent_name, const char *var_names[]);
void                 cs_subset_free(struct ConfigSubset **sub);
int                  cs_subset_lookup(struct ConfigSubset *sub, const char *name);

intptr_t cs_subset_native_get(const struct ConfigSubset *sub, int vid,                    struct Buffer *err);
int      cs_subset_native_set(const struct ConfigSubset *sub, int vid, intptr_t value,    struct Buffer *err);
int      cs_subset_reset     (const struct ConfigSubset *sub, int vid,                    struct Buffer *err);
int      cs_subset_string_get(const struct ConfigSubset *sub, int vid,                    struct Buffer *result);
int      cs_subset_string_set(const struct ConfigSubset *sub, int vid, const char *value, struct Buffer *err);

#endif /* MUTT_CONFIG_SUBSET_H */
