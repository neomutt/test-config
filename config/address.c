/**
 * @file
 * Type representing an email address
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
#include <stdint.h>
#include "address.h"
#include "lib/buffer.h"
#include "lib/debug.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "set.h"
#include "types.h"

/**
 * address_destroy - Destroy an Address object
 * @param cs   Config items
 * @param var  Variable to destroy
 * @param vdef Variable definition
 */
static void address_destroy(const struct ConfigSet *cs, void *var,
                            const struct VariableDef *vdef)
{
  if (!cs || !var || !vdef)
    return; /* LCOV_EXCL_LINE */

  struct Address **a = (struct Address **) var;
  if (!*a)
    return;

  address_free(a);
}

/**
 * address_string_set - Set an Address by string
 * @param cs    Config items
 * @param var   Variable to set
 * @param vdef  Variable definition
 * @param value Value to set
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int address_string_set(const struct ConfigSet *cs, void *var,
                              const struct VariableDef *vdef, const char *value,
                              struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct Address *addr = NULL;

  // An empty address "" will be stored as NULL
  if (value && (value[0] != '\0'))
  {
    addr = safe_calloc(1, sizeof(*addr));
    addr->personal = safe_strdup((const char *) value);
    addr->mailbox = safe_strdup("dummy1");
  }

  if (vdef->validator)
  {
    int rv = vdef->validator(cs, vdef, (intptr_t) addr, err);

    if (CSR_RESULT(rv) != CSR_SUCCESS)
    {
      address_destroy(cs, &addr, vdef);
      return rv | CSR_INV_VALIDATOR;
    }
  }

  address_destroy(cs, var, vdef);

  int result = CSR_SUCCESS;
  if (!addr)
    result |= CSR_SUC_EMPTY;

  *(struct Address **) var = addr;
  return result;
}

/**
 * address_string_get - Get an Address as a string
 * @param cs     Config items
 * @param var    Variable to get
 * @param vdef   Variable definition
 * @param result Buffer for results or error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int address_string_get(const struct ConfigSet *cs, void *var,
                              const struct VariableDef *vdef, struct Buffer *result)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct Address *a = *(struct Address **) var;
  if (!a)
    return CSR_SUCCESS | CSR_SUC_EMPTY; /* empty string */

  mutt_buffer_addstr(result, a->personal);
  return CSR_SUCCESS;
}

/**
 * address_dup - Create a copy of an Address object
 * @param addr Address to duplicate
 * @retval ptr New Address object
 */
static struct Address *address_dup(struct Address *addr)
{
  if (!addr)
    return NULL; /* LCOV_EXCL_LINE */

  struct Address *a = safe_calloc(1, sizeof(*a));
  a->personal = safe_strdup(addr->personal);
  a->mailbox = safe_strdup(addr->mailbox);
  return a;
}

/**
 * address_native_set - Set an Address config item by Address object
 * @param cs    Config items
 * @param var   Variable to set
 * @param vdef  Variable definition
 * @param value Address pointer
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int address_native_set(const struct ConfigSet *cs, void *var,
                              const struct VariableDef *vdef, intptr_t value,
                              struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  if (vdef->validator)
  {
    int rv = vdef->validator(cs, vdef, value, err);

    if (CSR_RESULT(rv) != CSR_SUCCESS)
      return rv | CSR_INV_VALIDATOR;
  }

  address_free(var);

  struct Address *addr = address_dup((struct Address *) value);

  int result = CSR_SUCCESS;
  if (!addr)
    result |= CSR_SUC_EMPTY;

  *(struct Address **) var = addr;
  return result;
}

/**
 * address_native_get - Get an Address object from an Address config item
 * @param cs   Config items
 * @param var  Variable to get
 * @param vdef Variable definition
 * @param err  Buffer for error messages
 * @retval intptr_t Address pointer
 */
static intptr_t address_native_get(const struct ConfigSet *cs, void *var,
                                   const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  struct Address *addr = *(struct Address **) var;

  return (intptr_t) addr;
}

/**
 * address_reset - Reset an Address to its initial value
 * @param cs   Config items
 * @param var  Variable to reset
 * @param vdef Variable definition
 * @param err  Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int address_reset(const struct ConfigSet *cs, void *var,
                         const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  address_destroy(cs, var, vdef);

  struct Address *a = NULL;
  const char *initial = (const char *) vdef->initial;

  if (initial)
    a = address_create(initial);

  int result = CSR_SUCCESS;
  if (!a)
    result |= CSR_SUC_EMPTY;

  *(struct Address **) var = a;
  return result;
}

/**
 * address_init - Register the Address config type
 * @param cs Config items
 */
void address_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_address = {
    "address",          address_string_set, address_string_get,
    address_native_set, address_native_get, address_reset,
    address_destroy,
  };
  cs_register_type(cs, DT_ADDRESS, &cst_address);
}

/**
 * address_create - Create an Address from a string
 * @param addr Email address to parse
 * @retval ptr New Address object
 */
struct Address *address_create(const char *addr)
{
  struct Address *a = safe_calloc(1, sizeof(*a));
  a->personal = safe_strdup(addr);
  a->mailbox = safe_strdup("dummy3");
  return a;
}

/**
 * address_free - Free an Address object
 * @param addr Address to free
 */
void address_free(struct Address **addr)
{
  if (!addr || !*addr)
    return; /* LCOV_EXCL_LINE */

  FREE(&(*addr)->personal);
  FREE(&(*addr)->mailbox);
  FREE(addr);
}
