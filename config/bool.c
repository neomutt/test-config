/**
 * @file
 * Type representing a boolean
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
 * @page config-bool Some bool
 *
 * LONG bool
 *
 * | Function        | Description
 * | :-------------- | :---------------------------------
 * | bool_init       | Register the Bool config type
 * | bool_native_get | Get a bool from a Bool config item
 * | bool_native_set | Set a Bool config item by bool
 * | bool_reset      | Reset a Bool to its initial value
 * | bool_string_get | Get a Bool as a string
 * | bool_string_set | Set a Bool by string
 */

#include "config.h"
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "lib/buffer.h"
#include "lib/debug.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "set.h"
#include "types.h"

const char *bool_values[] = {
  "no", "yes", "n", "y", "false", "true", "0", "1", "off", "on",
};

/**
 * bool_string_set - Set a Bool by string
 * @param cs    Config items
 * @param var   Variable to set
 * @param vdef  Variable definition
 * @param value Value to set
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int bool_string_set(const struct ConfigSet *cs, void *var,
                           const struct VariableDef *vdef, const char *value,
                           struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  int num = -1;
  for (unsigned int i = 0; i < mutt_array_size(bool_values); i++)
  {
    if (mutt_strcasecmp(bool_values[i], value) == 0)
    {
      num = i % 2;
      break;
    }
  }

  if (num < 0)
  {
    mutt_buffer_printf(err, "Invalid boolean value: %s", value);
    return CSR_ERR_INVALID | CSR_INV_TYPE;
  }

  if (vdef->validator)
  {
    int rv = vdef->validator(cs, vdef, (intptr_t) num, err);

    if (CSR_RESULT(rv) != CSR_SUCCESS)
      return rv | CSR_INV_VALIDATOR;
  }

  *(bool *) var = num;
  return CSR_SUCCESS;
}

/**
 * bool_string_get - Get a Bool as a string
 * @param cs     Config items
 * @param var    Variable to get
 * @param vdef   Variable definition
 * @param result Buffer for results or error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int bool_string_get(const struct ConfigSet *cs, void *var,
                           const struct VariableDef *vdef, struct Buffer *result)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  unsigned int index = *(bool *) var;
  if (index > 1)
  {
    mutt_debug(1, "Variable has an invalid value: %d\n", index);
    return CSR_ERR_INVALID | CSR_INV_TYPE;
  }

  mutt_buffer_addstr(result, bool_values[index]);
  return CSR_SUCCESS;
}

/**
 * bool_native_set - Set a Bool config item by bool
 * @param cs    Config items
 * @param var   Variable to set
 * @param vdef  Variable definition
 * @param value Bool value
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int bool_native_set(const struct ConfigSet *cs, void *var,
                           const struct VariableDef *vdef, intptr_t value,
                           struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  if ((value < 0) || (value > 1))
  {
    mutt_buffer_printf(err, "Invalid boolean value: %ld", value);
    return CSR_ERR_INVALID | CSR_INV_TYPE;
  }

  if (vdef->validator)
  {
    int rv = vdef->validator(cs, vdef, value, err);

    if (CSR_RESULT(rv) != CSR_SUCCESS)
      return rv | CSR_INV_VALIDATOR;
  }

  *(bool *) var = value;
  return CSR_SUCCESS;
}

/**
 * bool_native_get - Get a bool from a Bool config item
 * @param cs   Config items
 * @param var  Variable to get
 * @param vdef Variable definition
 * @param err  Buffer for error messages
 * @retval intptr_t Bool
 */
static intptr_t bool_native_get(const struct ConfigSet *cs, void *var,
                                const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  return *(bool *) var;
}

/**
 * bool_reset - Reset a Bool to its initial value
 * @param cs   Config items
 * @param var  Variable to reset
 * @param vdef Variable definition
 * @param err  Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int bool_reset(const struct ConfigSet *cs, void *var,
                      const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  *(bool *) var = vdef->initial;
  return CSR_SUCCESS;
}

/**
 * bool_init - Register the Bool config type
 * @param cs Config items
 */
void bool_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_bool = {
    "boolean", bool_string_set, bool_string_get, bool_native_set, bool_native_get, bool_reset, NULL,
  };
  cs_register_type(cs, DT_BOOL, &cst_bool);
}
