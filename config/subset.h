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

/**
 * struct ConfigSubset - XXX
 */
struct ConfigSubset
{
  char *name;                 ///< Name of Subset
  const struct ConfigSet *cs; ///< Parent ConfigSet
  const char **var_names;     ///< Array of the names of local config items
  size_t num_vars;            ///< Number of local config items
  struct HashElem **vars;     ///< Array of the HashElems of Subset config items
};

void                 config_subset_free(struct ConfigSubset **sub);
struct ConfigSubset *config_subset_new(const struct ConfigSet *cs, const char *name, const char *parent_name, const char *var_names[]);

#endif /* MUTT_CONFIG_SUBSET_H */
