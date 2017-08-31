/**
 * @file
 * Type representing a mailbox
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

const char *magic_values[] = { NULL, "mbox", "MMDF", "MH", "Maildir" };

static int magic_string_set(const struct ConfigSet *cs, void *var,
                            const struct VariableDef *vdef, const char *value,
                            struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  int num = -1;
  for (unsigned int i = 0; i < mutt_array_size(magic_values); i++)
  {
    if (mutt_strcasecmp(magic_values[i], value) == 0)
    {
      num = i;
      break;
    }
  }

  if (num < 1)
  {
    mutt_buffer_printf(err, "Invalid magic value: %s", value);
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

static int magic_string_get(const struct ConfigSet *cs, void *var,
                            const struct VariableDef *vdef, struct Buffer *result)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  unsigned int index = *(short *) var;
  if ((index < 1) || (index >= mutt_array_size(magic_values)))
  {
    mutt_debug(1, "Variable has an invalid value: %d\n", index);
    return CSR_ERR_INVALID | CSR_INV_TYPE;
  }

  mutt_buffer_addstr(result, magic_values[index]);
  return CSR_SUCCESS;
}

static int magic_native_set(const struct ConfigSet *cs, void *var,
                            const struct VariableDef *vdef, intptr_t value,
                            struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  if ((value < 1) || (value >= mutt_array_size(magic_values)))
  {
    mutt_buffer_printf(err, "Invalid magic value: %ld", value);
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

static intptr_t magic_native_get(const struct ConfigSet *cs, void *var,
                                 const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  return *(short *) var;
}

static int magic_reset(const struct ConfigSet *cs, void *var,
                       const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  *(short *) var = vdef->initial;
  return CSR_SUCCESS;
}

void magic_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_magic = {
    "magic", magic_string_set, magic_string_get, magic_native_set, magic_native_get, magic_reset, NULL,
  };
  cs_register_type(cs, DT_MAGIC, &cst_magic);
}
