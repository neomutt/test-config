/**
 * @file
 * Type representing a number
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
 * @page config-number Some number
 *
 * LONG number
 *
 * | Function          | Description
 * | :---------------- | :-----------------------------------
 * | number_init       | Register the Number config type
 * | number_native_get | Get an int from a Number config item
 * | number_native_set | Set a Number config item by int
 * | number_reset      | Reset a Number to its initial value
 * | number_string_get | Get a Number as a string
 * | number_string_set | Set a Number by string
 */

#include "config.h"
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "lib/buffer.h"
#include "lib/string2.h"
#include "set.h"
#include "types.h"

/**
 * number_string_set - Set a Number by string
 * @param cs    Config items
 * @param var   Variable to set
 * @param vdef  Variable definition
 * @param value Value to set
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int number_string_set(const struct ConfigSet *cs, void *var,
                             const struct VariableDef *vdef, const char *value,
                             struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  int num = 0;
  if (!value || !value[0] || mutt_atoi(value, &num) < 0)
  {
    mutt_buffer_printf(err, "Invalid number: %s", value);
    return CSR_ERR_INVALID | CSR_INV_TYPE;
  }

  if ((num < SHRT_MIN) || (num > SHRT_MAX))
  {
    mutt_buffer_printf(err, "Number is too big: %s", value);
    return CSR_ERR_INVALID | CSR_INV_TYPE;
  }

  if (vdef->validator)
  {
    int rv = vdef->validator(cs, vdef, (intptr_t) num, err);

    if (CSR_RESULT(rv) != CSR_SUCCESS)
      return rv | CSR_INV_VALIDATOR;
  }

  *(short *) var = num;
  return CSR_SUCCESS;
}

/**
 * number_string_get - Get a Number as a string
 * @param cs     Config items
 * @param var    Variable to get
 * @param vdef   Variable definition
 * @param result Buffer for results or error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int number_string_get(const struct ConfigSet *cs, void *var,
                             const struct VariableDef *vdef, struct Buffer *result)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  mutt_buffer_printf(result, "%d", *(short *) var);
  return CSR_SUCCESS;
}

/**
 * number_native_set - Set a Number config item by int
 * @param cs    Config items
 * @param var   Variable to set
 * @param vdef  Variable definition
 * @param value Number
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int number_native_set(const struct ConfigSet *cs, void *var,
                             const struct VariableDef *vdef, intptr_t value,
                             struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  if ((value < SHRT_MIN) || (value > SHRT_MAX))
  {
    mutt_buffer_printf(err, "Invalid number: %ld", value);
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
 * number_native_get - Get an int from a Number config item
 * @param cs   Config items
 * @param var  Variable to get
 * @param vdef Variable definition
 * @param err  Buffer for error messages
 * @retval intptr_t Number
 */
static intptr_t number_native_get(const struct ConfigSet *cs, void *var,
                                  const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  return *(short *) var;
}

/**
 * number_reset - Reset a Number to its initial value
 * @param cs   Config items
 * @param var  Variable to reset
 * @param vdef Variable definition
 * @param err  Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int number_reset(const struct ConfigSet *cs, void *var,
                        const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  *(short *) var = vdef->initial;
  return CSR_SUCCESS;
}

/**
 * number_init - Register the Number config type
 * @param cs Config items
 */
void number_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_number = {
    "number", number_string_set, number_string_get, number_native_set, number_native_get, number_reset, NULL,
  };
  cs_register_type(cs, DT_NUMBER, &cst_number);
}
