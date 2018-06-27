/**
 * @file
 * Type representing an email address
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
 * @page config-address Type: Email address
 *
 * Type representing an email address.
 */

#include "config.h"
#include <stddef.h>
#include <limits.h>
#include <stdint.h>
#include "mutt/address.h"
#include "mutt/buffer.h"
#include "mutt/memory.h"
#include "mutt/string2.h"
#include "address.h"
#include "set.h"
#include "types.h"

/**
 * address_destroy - Destroy an Address object
 * @param cs   Config items
 * @param var  Variable to destroy
 * @param cdef Variable definition
 */
static void address_destroy(const struct ConfigSet *cs, void *var, const struct ConfigDef *cdef)
{
  if (!cs || !var || !cdef)
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
 * @param cdef  Variable definition
 * @param value Value to set
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 *
 * If var is NULL, then the config item's initial value will be set.
 */
static int address_string_set(const struct ConfigSet *cs, void *var, struct ConfigDef *cdef,
                              const char *value, struct Buffer *err)
{
  if (!cs || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct Address *addr = NULL;

  /* An empty address "" will be stored as NULL */
  if (var && value && (value[0] != '\0'))
  {
    addr = mutt_addr_parse_list(NULL, value);
  }

  int rc = CSR_SUCCESS;

  if (var)
  {
    if (cdef->validator)
    {
      rc = cdef->validator(cs, cdef, (intptr_t) addr, err);

      if (CSR_RESULT(rc) != CSR_SUCCESS)
      {
        address_destroy(cs, &addr, cdef);
        return (rc | CSR_INV_VALIDATOR);
      }
    }

    /* ordinary variable setting */
    address_destroy(cs, var, cdef);

    *(struct Address **) var = addr;

    if (!addr)
      rc |= CSR_SUC_EMPTY;
  }
  else
  {
    /* set the default/initial value */
    if (cdef->type & DT_INITIAL_SET)
      FREE(&cdef->initial);

    cdef->type |= DT_INITIAL_SET;
    cdef->initial = IP mutt_str_strdup(value);
  }

  return rc;
}

/**
 * address_string_get - Get an Address as a string
 * @param cs     Config items
 * @param var    Variable to get
 * @param cdef   Variable definition
 * @param result Buffer for results or error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 *
 * If var is NULL, then the config item's initial value will be returned.
 */
static int address_string_get(const struct ConfigSet *cs, void *var,
                              const struct ConfigDef *cdef, struct Buffer *result)
{
  if (!cs || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  char tmp[HUGE_STRING] = "";
  const char *str = NULL;

  if (var)
  {
    struct Address *a = *(struct Address **) var;
    if (a)
    {
      mutt_addr_write(tmp, sizeof(tmp), a, false);
      str = tmp;
    }
  }
  else
  {
    str = (char *) cdef->initial;
  }

  if (!str)
    return (CSR_SUCCESS | CSR_SUC_EMPTY); /* empty string */

  mutt_buffer_addstr(result, str);
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

  struct Address *a = mutt_mem_calloc(1, sizeof(*a));
  a->personal = mutt_str_strdup(addr->personal);
  a->mailbox = mutt_str_strdup(addr->mailbox);
  return a;
}

/**
 * address_native_set - Set an Address config item by Address object
 * @param cs    Config items
 * @param var   Variable to set
 * @param cdef  Variable definition
 * @param value Address pointer
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int address_native_set(const struct ConfigSet *cs, void *var,
                              const struct ConfigDef *cdef, intptr_t value,
                              struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  int rc;

  if (cdef->validator)
  {
    rc = cdef->validator(cs, cdef, value, err);

    if (CSR_RESULT(rc) != CSR_SUCCESS)
      return (rc | CSR_INV_VALIDATOR);
  }

  address_free(var);

  struct Address *addr = address_dup((struct Address *) value);

  rc = CSR_SUCCESS;
  if (!addr)
    rc |= CSR_SUC_EMPTY;

  *(struct Address **) var = addr;
  return rc;
}

/**
 * address_native_get - Get an Address object from an Address config item
 * @param cs   Config items
 * @param var  Variable to get
 * @param cdef Variable definition
 * @param err  Buffer for error messages
 * @retval intptr_t Address pointer
 */
static intptr_t address_native_get(const struct ConfigSet *cs, void *var,
                                   const struct ConfigDef *cdef, struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  struct Address *addr = *(struct Address **) var;

  return (intptr_t) addr;
}

/**
 * address_reset - Reset an Address to its initial value
 * @param cs   Config items
 * @param var  Variable to reset
 * @param cdef Variable definition
 * @param err  Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int address_reset(const struct ConfigSet *cs, void *var,
                         const struct ConfigDef *cdef, struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct Address *a = NULL;
  const char *initial = (const char *) cdef->initial;

  if (initial)
    a = address_create(initial);

  int rc = CSR_SUCCESS;

  if (cdef->validator)
  {
    rc = cdef->validator(cs, cdef, (intptr_t) a, err);

    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      address_destroy(cs, &a, cdef);
      return (rc | CSR_INV_VALIDATOR);
    }
  }

  if (!a)
    rc |= CSR_SUC_EMPTY;

  address_destroy(cs, var, cdef);

  *(struct Address **) var = a;
  return rc;
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
  struct Address *a = mutt_mem_calloc(1, sizeof(*a));
  // a->personal = mutt_str_strdup(addr);
  a->mailbox = mutt_str_strdup(addr);
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
