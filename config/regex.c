/**
 * @file
 * Type representing a regular expression
 *
 * @authors
 * Copyright (C) 1996-2000 Michael R. Elkins <me@mutt.org>
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
 * @page config-regex Some regex
 *
 * LONG regex
 *
 * | Function         | Description
 * | :--------------- | :------------------------------------------
 * | regex_create     | Create an Regex from a string
 * | regex_destroy    | Destroy a Regex object
 * | regex_dup        | Create a copy of a Regex object
 * | regex_free       | Free a Regex object
 * | regex_init       | Register the Regex config type
 * | regex_native_get | Get a Regex object from a Regex config item
 * | regex_native_set | Set a Regex config item by Regex object
 * | regex_reset      | Reset a Regex to its initial value
 * | regex_string_get | Get a Regex as a string
 * | regex_string_set | Set a Regex by string
 */

#include "config.h"
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "regex2.h"
#include "set.h"
#include "types.h"

/**
 * regex_destroy - Destroy a Regex object
 * @param cs   Config items
 * @param var  Variable to destroy
 * @param cdef Variable definition
 */
static void regex_destroy(const struct ConfigSet *cs, void *var, const struct ConfigDef *cdef)
{
  if (!cs || !var || !cdef)
    return; /* LCOV_EXCL_LINE */

  struct Regex **r = (struct Regex **) var;
  if (!*r)
    return;

  regex_free(r);
}

/**
 * regex_string_set - Set a Regex by string
 * @param cs    Config items
 * @param var   Variable to set
 * @param cdef  Variable definition
 * @param value Value to set
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int regex_string_set(const struct ConfigSet *cs, void *var,
                            const struct ConfigDef *cdef, const char *value,
                            struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct Regex *r = NULL;
  if (value)
  {
    r = safe_calloc(1, sizeof(*r));
    r->pattern = safe_strdup(value);
    r->regex = NULL; // regenerate r->regex
  }

  if (cdef->validator)
  {
    int rv = cdef->validator(cs, cdef, (intptr_t) r, err);

    if (CSR_RESULT(rv) != CSR_SUCCESS)
    {
      regex_free(&r);
      return rv | CSR_INV_VALIDATOR;
    }
  }

  regex_destroy(cs, var, cdef);

  int result = CSR_SUCCESS;
  if (!r)
    result |= CSR_SUC_EMPTY;

  *(struct Regex **) var = r;
  return result;
}

/**
 * regex_string_get - Get a Regex as a string
 * @param cs     Config items
 * @param var    Variable to get
 * @param cdef   Variable definition
 * @param result Buffer for results or error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int regex_string_get(const struct ConfigSet *cs, void *var,
                            const struct ConfigDef *cdef, struct Buffer *result)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct Regex *r = *(struct Regex **) var;
  if (!r)
    return CSR_SUCCESS | CSR_SUC_EMPTY;

  mutt_buffer_addstr(result, r->pattern);
  return CSR_SUCCESS;
}

/**
 * regex_dup - Create a copy of a Regex object
 * @param r Regex to duplicate
 * @retval ptr New Regex object
 */
static struct Regex *regex_dup(struct Regex *r)
{
  if (!r)
    return NULL; /* LCOV_EXCL_LINE */

  struct Regex *copy = safe_calloc(1, sizeof(*copy));
  copy->pattern = safe_strdup(r->pattern);
  return copy;
}

/**
 * regex_native_set - Set a Regex config item by Regex object
 * @param cs    Config items
 * @param var   Variable to set
 * @param cdef  Variable definition
 * @param value Regex pointer
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int regex_native_set(const struct ConfigSet *cs, void *var,
                            const struct ConfigDef *cdef, intptr_t value,
                            struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  if (cdef->validator)
  {
    int rv = cdef->validator(cs, cdef, value, err);

    if (CSR_RESULT(rv) != CSR_SUCCESS)
      return rv | CSR_INV_VALIDATOR;
  }

  regex_free(var);

  struct Regex *r = regex_dup((struct Regex *) value);

  int result = CSR_SUCCESS;
  if (!r)
    result |= CSR_SUC_EMPTY;

  *(struct Regex **) var = r;
  return result;
}

/**
 * regex_native_get - Get a Regex object from a Regex config item
 * @param cs   Config items
 * @param var  Variable to get
 * @param cdef Variable definition
 * @param err  Buffer for error messages
 * @retval intptr_t Regex pointer
 */
static intptr_t regex_native_get(const struct ConfigSet *cs, void *var,
                                 const struct ConfigDef *cdef, struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  struct Regex *r = *(struct Regex **) var;

  return (intptr_t) r;
}

/**
 * regex_reset - Reset a Regex to its initial value
 * @param cs   Config items
 * @param var  Variable to reset
 * @param cdef Variable definition
 * @param err  Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int regex_reset(const struct ConfigSet *cs, void *var,
                       const struct ConfigDef *cdef, struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  regex_destroy(cs, var, cdef);

  struct Regex *r = NULL;
  const char *initial = (const char *) cdef->initial;

  if (initial)
  {
    r = safe_calloc(1, sizeof(*r));
    r->pattern = safe_strdup((char *) cdef->initial);
    r->regex = NULL; // regenerate r->regex
  }

  int result = CSR_SUCCESS;
  if (!r)
    result |= CSR_SUC_EMPTY;

  *(struct Regex **) var = r;
  return result;
}

/**
 * regex_init - Register the Regex config type
 * @param cs Config items
 */
void regex_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_regex = {
    "regex",          regex_string_set, regex_string_get, regex_native_set,
    regex_native_get, regex_reset,      regex_destroy,
  };
  cs_register_type(cs, DT_REGEX, &cst_regex);
}

/**
 * regex_create - Create an Regex from a string
 * @param str Regular expression
 * @retval ptr New Regex object
 */
struct Regex *regex_create(const char *str)
{
  struct Regex *r = safe_calloc(1, sizeof(*r));
  r->pattern = safe_strdup(str);
  return r;
}

/**
 * regex_free - Free a Regex object
 * @param r Regex to free
 */
void regex_free(struct Regex **r)
{
  if (!r || !*r)
    return; /* LCOV_EXCL_LINE */

  FREE(&(*r)->pattern);
  //regfree(r->regex)
  FREE(r);
}
