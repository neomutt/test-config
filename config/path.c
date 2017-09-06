/**
 * @file
 * Type representing a path
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

/**
 * @page config-path Some path
 *
 * LONG path
 *
 * | Function        | Description
 * | :-------------- | :-----------------------------------
 * | path_destroy    | Destroy a Path
 * | path_init       | Register the Path config type
 * | path_native_get | Get a string from a Path config item
 * | path_native_set | Set a Path config item by string
 * | path_reset      | Reset a Path to its initial value
 * | path_string_get | Get a Path as a string
 * | path_string_set | Set a Path by string
 */

#include "config.h"
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "set.h"
#include "types.h"

/**
 * path_destroy - Destroy a Path
 * @param cs   Config items
 * @param var  Variable to destroy
 * @param cdef Variable definition
 */
static void path_destroy(const struct ConfigSet *cs, void *var, const struct ConfigDef *cdef)
{
  if (!cs || !var || !cdef)
    return; /* LCOV_EXCL_LINE */

  /* Don't free strings from the var definition */
  if (*(char **) var == (char *) cdef->initial)
    return;

  FREE(var);
}

/**
 * path_string_set - Set a Path by string
 * @param cs    Config items
 * @param var   Variable to set
 * @param cdef  Variable definition
 * @param value Value to set
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int path_string_set(const struct ConfigSet *cs, void *var,
                           const struct ConfigDef *cdef, const char *value,
                           struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  /* Store empty strings as NULL */
  if (value && (value[0] == '\0'))
    value = NULL;

  if (cdef->validator)
  {
    int rv = cdef->validator(cs, cdef, (intptr_t) value, err);

    if (CSR_RESULT(rv) != CSR_SUCCESS)
      return rv | CSR_INV_VALIDATOR;
  }

  path_destroy(cs, var, cdef);

  const char *str = safe_strdup(value);
  int result = CSR_SUCCESS;
  if (!str)
    result |= CSR_SUC_EMPTY;

  *(const char **) var = str;
  return result;
}

/**
 * path_string_get - Get a Path as a string
 * @param cs     Config items
 * @param var    Variable to get
 * @param cdef   Variable definition
 * @param result Buffer for results or error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int path_string_get(const struct ConfigSet *cs, void *var,
                           const struct ConfigDef *cdef, struct Buffer *result)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  const char *str = *(const char **) var;
  if (!str)
    return CSR_SUCCESS | CSR_SUC_EMPTY; /* empty string */

  mutt_buffer_addstr(result, str);
  return CSR_SUCCESS;
}

/**
 * path_native_set - Set a Path config item by string
 * @param cs    Config items
 * @param var   Variable to set
 * @param cdef  Variable definition
 * @param value Path string
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int path_native_set(const struct ConfigSet *cs, void *var,
                           const struct ConfigDef *cdef, intptr_t value,
                           struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  const char *str = (const char *) value;

  /* Store empty strings as NULL */
  if (str && (str[0] == '\0'))
    value = 0;

  if (cdef->validator)
  {
    int rv = cdef->validator(cs, cdef, value, err);

    if (CSR_RESULT(rv) != CSR_SUCCESS)
      return rv | CSR_INV_VALIDATOR;
  }

  path_destroy(cs, var, cdef);

  str = safe_strdup((const char *) value);
  int result = CSR_SUCCESS;
  if (!str)
    result |= CSR_SUC_EMPTY;

  *(const char **) var = str;
  return result;
}

/**
 * path_native_get - Get a string from a Path config item
 * @param cs   Config items
 * @param var  Variable to get
 * @param cdef Variable definition
 * @param err  Buffer for error messages
 * @retval intptr_t Path string
 */
static intptr_t path_native_get(const struct ConfigSet *cs, void *var,
                                const struct ConfigDef *cdef, struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  const char *str = *(const char **) var;

  return (intptr_t) str;
}

/**
 * path_reset - Reset a Path to its initial value
 * @param cs   Config items
 * @param var  Variable to reset
 * @param cdef Variable definition
 * @param err  Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int path_reset(const struct ConfigSet *cs, void *var,
                      const struct ConfigDef *cdef, struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  path_destroy(cs, var, cdef);

  const char *path = (const char *) cdef->initial;

  int result = CSR_SUCCESS;
  if (!path)
    result |= CSR_SUC_EMPTY;

  *(const char **) var = path;
  return result;
}

/**
 * path_init - Register the Path config type
 * @param cs Config items
 */
void path_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_path = {
    "path",          path_string_set, path_string_get, path_native_set,
    path_native_get, path_reset,      path_destroy,
  };
  cs_register_type(cs, DT_PATH, &cst_path);
}
