/**
 * @file
 * Type representing a sort option
 *
 * @authors
 * Copyright (C) 2017 Richard Russon <rich@flatcap.org>
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

#include "config.h"
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "sort.h"
#include "lib/buffer.h"
#include "lib/debug.h"
#include "lib/mapping.h"
#include "lib/string2.h"
#include "set.h"
#include "types.h"

const struct Mapping SortAliasMethods[] = {
  { "alias", SORT_ALIAS }, { "address", SORT_ADDRESS }, { "unsorted", SORT_ORDER }, { NULL, 0 },
};

const struct Mapping SortAuxMethods[] = {
  { "date", SORT_DATE },
  { "date-sent", SORT_DATE },
  { "date-received", SORT_RECEIVED },
  { "mailbox-order", SORT_ORDER },
  { "subject", SORT_SUBJECT },
  { "from", SORT_FROM },
  { "size", SORT_SIZE },
  { "threads", SORT_DATE },
  { "to", SORT_TO },
  { "score", SORT_SCORE },
  { "spam", SORT_SPAM },
  { "label", SORT_LABEL },
  { NULL, 0 },
};

const struct Mapping SortBrowserMethods[] = {
  { "alpha", SORT_SUBJECT },  { "count", SORT_COUNT },
  { "date", SORT_DATE },      { "desc", SORT_DESC },
  { "new", SORT_UNREAD },     { "size", SORT_SIZE },
  { "unsorted", SORT_ORDER }, { NULL, 0 },
};

const struct Mapping SortKeyMethods[] = {
  { "address", SORT_ADDRESS }, { "date", SORT_DATE }, { "keyid", SORT_KEYID },
  { "trust", SORT_TRUST },     { NULL, 0 },
};

const struct Mapping SortMethods[] = {
  { "date", SORT_DATE },
  { "date-sent", SORT_DATE },
  { "date-received", SORT_RECEIVED },
  { "mailbox-order", SORT_ORDER },
  { "subject", SORT_SUBJECT },
  { "from", SORT_FROM },
  { "size", SORT_SIZE },
  { "threads", SORT_THREADS },
  { "to", SORT_TO },
  { "score", SORT_SCORE },
  { "spam", SORT_SPAM },
  { "label", SORT_LABEL },
  { NULL, 0 },
};

const struct Mapping SortSidebarMethods[] = {
  { "alpha", SORT_PATH },
  { "count", SORT_COUNT },
  { "desc", SORT_DESC },
  { "flagged", SORT_FLAGGED },
  { "mailbox-order", SORT_ORDER },
  { "name", SORT_PATH },
  { "new", SORT_UNREAD },
  { "path", SORT_PATH },
  { "unread", SORT_UNREAD },
  { "unsorted", SORT_ORDER },
  { NULL, 0 },
};

/**
 * find_string - Lookup a sort string
 * @param map  Mapping between strings and constants
 * @param type Sort code to lookup, e.g. #SORT_ADDRESS
 * @retval ptr String for sort ID
 * @retval NULL No match
 */
static const char *find_string(const struct Mapping *map, int value)
{
  if (!map)
    return NULL; /* LCOV_EXCL_LINE */

  for (int i = 0; map[i].name; i++)
    if (map[i].value == value)
      return map[i].name;

  return NULL;
}

/**
 * find_id - Lookup a sort ID
 * @param map Mapping between strings and constants
 * @param str Sort string to find, e.g. "alpha"
 * @retval int ID matching string, e.g. #SORT_ADDRESS
 * @retval int -1 No match
 */
static int find_id(const struct Mapping *map, const char *str)
{
  if (!map || !str)
    return -1; /* LCOV_EXCL_LINE */

  for (int i = 0; map[i].name; i++)
    if (mutt_strcasecmp(map[i].name, str) == 0)
      return map[i].value;

  return -1;
}

/**
 * sort_string_set - Set a Sort by string
 * @param cs    Config items
 * @param var   Variable to set
 * @param vdef  Variable definition
 * @param value Value to set
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int sort_string_set(const struct ConfigSet *cs, void *var,
                           const struct VariableDef *vdef, const char *value,
                           struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  intptr_t id = -1;

  switch (vdef->type & DT_SUBTYPE_MASK)
  {
    case DT_SORT_INDEX:
      id = find_id(SortMethods, value);
      break;
    case DT_SORT_ALIAS:
      id = find_id(SortAliasMethods, value);
      break;
    case DT_SORT_AUX:
      id = find_id(SortAuxMethods, value);
      break;
    case DT_SORT_BROWSER:
      id = find_id(SortBrowserMethods, value);
      break;
    case DT_SORT_KEYS:
      id = find_id(SortKeyMethods, value);
      break;
    case DT_SORT_SIDEBAR:
      id = find_id(SortSidebarMethods, value);
      break;
    default:
      mutt_debug(1, "Invalid sort type: %ld\n", vdef->type & DT_SUBTYPE_MASK);
      return CSR_ERR_CODE;
      break;
  }

  if (id < 0)
  {
    mutt_buffer_printf(err, "Invalid sort name: %s", value);
    return CSR_ERR_INVALID | CSR_INV_TYPE;
  }

  if (vdef->validator)
  {
    int rv = vdef->validator(cs, vdef, (intptr_t) id, err);

    if (CSR_RESULT(rv) != CSR_SUCCESS)
      return rv | CSR_INV_VALIDATOR;
  }

  *(short *) var = id;
  return CSR_SUCCESS;
}

/**
 * sort_string_get - Get a Sort as a string
 * @param cs     Config items
 * @param var    Variable to get
 * @param vdef   Variable definition
 * @param result Buffer for results or error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int sort_string_get(const struct ConfigSet *cs, void *var,
                           const struct VariableDef *vdef, struct Buffer *result)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  int sort = *(short *) var;

  const char *str = NULL;

  switch (vdef->type & DT_SUBTYPE_MASK)
  {
    case DT_SORT_INDEX:
      str = find_string(SortMethods, sort);
      break;
    case DT_SORT_ALIAS:
      str = find_string(SortAliasMethods, sort);
      break;
    case DT_SORT_AUX:
      str = find_string(SortAuxMethods, sort);
      break;
    case DT_SORT_BROWSER:
      str = find_string(SortBrowserMethods, sort);
      break;
    case DT_SORT_KEYS:
      str = find_string(SortKeyMethods, sort);
      break;
    case DT_SORT_SIDEBAR:
      str = find_string(SortSidebarMethods, sort);
      break;
    default:
      mutt_debug(1, "Invalid sort type: %ld\n", vdef->type & DT_SUBTYPE_MASK);
      return CSR_ERR_CODE;
      break;
  }

  if (!str)
  {
    mutt_debug(1, "Variable has an invalid value: %d/%d\n",
               vdef->type & DT_SUBTYPE_MASK, sort);
    return CSR_ERR_INVALID | CSR_INV_TYPE;
  }

  mutt_buffer_addstr(result, str);
  return CSR_SUCCESS;
}

/**
 * sort_native_set - Set a Sort config item by int
 * @param cs    Config items
 * @param var   Variable to set
 * @param vdef  Variable definition
 * @param value Sort value
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int sort_native_set(const struct ConfigSet *cs, void *var,
                           const struct VariableDef *vdef, intptr_t value,
                           struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  const char *str = NULL;

  switch (vdef->type & DT_SUBTYPE_MASK)
  {
    case DT_SORT_INDEX:
      str = find_string(SortMethods, value);
      break;
    case DT_SORT_ALIAS:
      str = find_string(SortAliasMethods, value);
      break;
    case DT_SORT_AUX:
      str = find_string(SortAuxMethods, value);
      break;
    case DT_SORT_BROWSER:
      str = find_string(SortBrowserMethods, value);
      break;
    case DT_SORT_KEYS:
      str = find_string(SortKeyMethods, value);
      break;
    case DT_SORT_SIDEBAR:
      str = find_string(SortSidebarMethods, value);
      break;
    default:
      mutt_debug(1, "Invalid sort type: %ld\n", vdef->type & DT_SUBTYPE_MASK);
      return CSR_ERR_CODE;
      break;
  }

  if (!str)
  {
    mutt_buffer_printf(err, "Invalid sort type: %ld", value);
    return CSR_ERR_INVALID | CSR_INV_TYPE;
  }

  if (vdef->validator)
  {
    int rv = vdef->validator(cs, vdef, value, err);

    if (CSR_RESULT(rv) != CSR_SUCCESS)
      return rv | CSR_INV_VALIDATOR;
  }

  *(short *) var = value;
  return CSR_SUCCESS;
}

/**
 * sort_native_get - Get an int from a Sort config item
 * @param cs   Config items
 * @param var  Variable to get
 * @param vdef Variable definition
 * @param err  Buffer for error messages
 * @retval intptr_t Sort ID
 */
static intptr_t sort_native_get(const struct ConfigSet *cs, void *var,
                                const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  return *(short *) var;
}

/**
 * sort_reset - Reset a Sort to its initial value
 * @param cs   Config items
 * @param var  Variable to reset
 * @param vdef Variable definition
 * @param err  Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int sort_reset(const struct ConfigSet *cs, void *var,
                      const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  *(short *) var = vdef->initial;
  return CSR_SUCCESS;
}

/**
 * sort_init - Register the Sort config type
 * @param cs Config items
 */
void sort_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_sort = {
    "sort", sort_string_set, sort_string_get, sort_native_set, sort_native_get, sort_reset, NULL,
  };
  cs_register_type(cs, DT_SORT, &cst_sort);
}
