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

static void regex_destroy(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef)
{
  if (!cs || !var || !vdef)
    return; /* LCOV_EXCL_LINE */

  struct Regex **r = (struct Regex **) var;
  if (!*r)
    return;

  regex_free(r);
}

static int regex_string_set(const struct ConfigSet *cs, void *var,
                            const struct VariableDef *vdef, const char *value,
                            struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct Regex *r = NULL;
  if (value)
  {
    r = safe_calloc(1, sizeof(*r));
    r->pattern = safe_strdup(value);
    r->regex = NULL; // regenerate r->regex
  }

  if (vdef->validator)
  {
    int rv = vdef->validator(cs, vdef, (intptr_t) r, err);

    if (CSR_RESULT(rv) != CSR_SUCCESS)
    {
      regex_free(&r);
      return rv | CSR_INV_VALIDATOR;
    }
  }

  regex_destroy(cs, var, vdef);

  int result = CSR_SUCCESS;
  if (!r)
    result |= CSR_SUC_EMPTY;

  *(struct Regex **) var = r;
  return result;
}

static int regex_string_get(const struct ConfigSet *cs, void *var,
                            const struct VariableDef *vdef, struct Buffer *result)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct Regex *r = *(struct Regex **) var;
  if (!r)
    return CSR_SUCCESS | CSR_SUC_EMPTY;

  mutt_buffer_addstr(result, r->pattern);
  return CSR_SUCCESS;
}

static struct Regex *regex_dup(struct Regex *r)
{
  if (!r)
    return NULL; /* LCOV_EXCL_LINE */

  struct Regex *copy = safe_calloc(1, sizeof(*copy));
  copy->pattern = safe_strdup(r->pattern);
  return copy;
}

static int regex_native_set(const struct ConfigSet *cs, void *var,
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

  regex_free(var);

  struct Regex *r = regex_dup((struct Regex *) value);

  int result = CSR_SUCCESS;
  if (!r)
    result |= CSR_SUC_EMPTY;

  *(struct Regex **) var = r;
  return result;
}

static intptr_t regex_native_get(const struct ConfigSet *cs, void *var,
                                 const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  struct Regex *r = *(struct Regex **) var;

  return (intptr_t) r;
}

static int regex_reset(const struct ConfigSet *cs, void *var,
                       const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  regex_destroy(cs, var, vdef);

  struct Regex *r = NULL;
  const char *initial = (const char *) vdef->initial;

  if (initial)
  {
    r = safe_calloc(1, sizeof(*r));
    r->pattern = safe_strdup((char *) vdef->initial);
    r->regex = NULL; // regenerate r->regex
  }

  int result = CSR_SUCCESS;
  if (!r)
    result |= CSR_SUC_EMPTY;

  *(struct Regex **) var = r;
  return result;
}

void regex_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_regex = {
    "regex",          regex_string_set, regex_string_get, regex_native_set,
    regex_native_get, regex_reset,      regex_destroy,
  };
  cs_register_type(cs, DT_REGEX, &cst_regex);
}

struct Regex *regex_create(const char *str)
{
  struct Regex *r = safe_calloc(1, sizeof(*r));
  r->pattern = safe_strdup(str);
  return r;
}

void regex_free(struct Regex **r)
{
  if (!r || !*r)
    return; /* LCOV_EXCL_LINE */

  FREE(&(*r)->pattern);
  //regfree(r->regex)
  FREE(r);
}
