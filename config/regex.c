/**
 * @file
 * Type representing a regular expression
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
 * @page config-regex Some regex
 *
 * LONG regex
 *
 * | Function           | Description
 * | :----------------- | :------------------------------------------
 * | regex_create()     | Create an Regex from a string
 * | regex_destroy()    | Destroy a Regex object
 * | regex_free()       | Free a Regex object
 * | regex_init()       | Register the Regex config type
 * | regex_native_get() | Get a Regex object from a Regex config item
 * | regex_native_set() | Set a Regex config item by Regex object
 * | regex_reset()      | Reset a Regex to its initial value
 * | regex_string_get() | Get a Regex as a string
 * | regex_string_set() | Set a Regex by string
 */

#include "config.h"
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "mutt/buffer.h"
#include "mutt/mbyte.h"
#include "mutt/memory.h"
#include "mutt/string2.h"
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

  /* Store empty strings as NULL */
  if (value && (value[0] == '\0'))
    value = NULL;

  struct Regex *r = NULL;

  if (value)
  {
    r = regex_create(value, cdef->type, err);
    if (!r)
      return CSR_ERR_INVALID;
  }

  if (cdef->validator)
  {
    int rc = cdef->validator(cs, cdef, (intptr_t) r, err);

    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      regex_free(&r);
      return rc | CSR_INV_VALIDATOR;
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
 *
 * If var is NULL, then the initial value is returned.
 */
static int regex_string_get(const struct ConfigSet *cs, void *var,
                            const struct ConfigDef *cdef, struct Buffer *result)
{
  if (!cs || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  const char *str = NULL;

  if (var)
  {
    struct Regex *r = *(struct Regex **) var;
    if (r)
      str = r->pattern;
  }
  else
  {
    str = (char *) cdef->initial;
  }

  if (!str)
    return CSR_SUCCESS | CSR_SUC_EMPTY; /* empty string */

  mutt_buffer_addstr(result, str);
  return CSR_SUCCESS;
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
                            const struct ConfigDef *cdef, intptr_t value, struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  if (cdef->validator)
  {
    int rc = cdef->validator(cs, cdef, value, err);

    if (CSR_RESULT(rc) != CSR_SUCCESS)
      return rc | CSR_INV_VALIDATOR;
  }

  regex_free(var);

  int result = CSR_SUCCESS;
  struct Regex *orig = (struct Regex *) value;
  struct Regex *r = NULL;

  if (orig && orig->pattern)
  {
    r = regex_create(orig->pattern, cdef->flags, err);
    if (!r)
      result = CSR_ERR_INVALID;
  }
  else
  {
    result |= CSR_SUC_EMPTY;
  }

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

  int result = CSR_SUCCESS;

  if (initial)
  {
    r = regex_create(initial, cdef->type, err);
    if (!r)
      result = CSR_ERR_INVALID;
  }
  else
  {
    result |= CSR_SUC_EMPTY;
  }

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
 * @param str   Regular expression
 * @param flags Type flags, e.g. #DT_REGEX_MATCH_CASE
 * @param err   Buffer for error messages
 * @retval ptr New Regex object
 * @retval NULL Error
 */
struct Regex *regex_create(const char *str, int flags, struct Buffer *err)
{
  if (!str)
    return NULL;

  int rflags = 0;
  struct Regex *reg = mutt_mem_calloc(1, sizeof(struct Regex));

  reg->regex = mutt_mem_calloc(1, sizeof(regex_t));
  reg->pattern = mutt_str_strdup(str);

  /* Should we use smart case matching? */
  if (((flags & DT_REGEX_MATCH_CASE) == 0) && mutt_mb_is_lower(str))
    rflags |= REG_ICASE;

  /* Is a prefix of '!' allowed? */
  if (((flags & DT_REGEX_ALLOW_NOT) != 0) && (str[0] == '!'))
  {
    reg->not = true;
    str++;
  }

  int rc = REGCOMP(reg->regex, str, rflags);
  if ((rc != 0) && err)
  {
    regerror(rc, reg->regex, err->data, err->dsize);
    regex_free(&reg);
    return NULL;
  }

  return reg;
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
  if ((*r)->regex)
    regfree((*r)->regex);
  FREE(r);
}
