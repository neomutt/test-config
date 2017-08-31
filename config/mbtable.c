/**
 * @file
 * Type representing a multibyte character table
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
#include <wchar.h>
#include "mbtable.h"
#include "lib/buffer.h"
#include "lib/debug.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "set.h"
#include "types.h"

static struct MbTable *mbtable_parse(const char *s)
{
  struct MbTable *t = NULL;
  size_t slen, k;
  mbstate_t mbstate;
  char *d = NULL;

  slen = mutt_strlen(s);
  if (!slen)
    return NULL;

  t = safe_calloc(1, sizeof(struct MbTable));

  t->orig_str = safe_strdup(s);
  /* This could be more space efficient.  However, being used on tiny
   * strings (Tochars and StChars), the overhead is not great. */
  t->chars = safe_calloc(slen, sizeof(char *));
  d = t->segmented_str = safe_calloc(slen * 2, sizeof(char));

  memset(&mbstate, 0, sizeof(mbstate));
  while (slen && (k = mbrtowc(NULL, s, slen, &mbstate)))
  {
    if (k == (size_t)(-1) || k == (size_t)(-2))
    {
      mutt_debug(1, "mbtable_parse: mbrtowc returned %d converting %s in %s\n",
                 (k == (size_t)(-1)) ? -1 : -2, s, t->orig_str);
      if (k == (size_t)(-1))
        memset(&mbstate, 0, sizeof(mbstate));
      k = (k == (size_t)(-1)) ? 1 : slen;
    }

    slen -= k;
    t->chars[t->len++] = d;
    while (k--)
      *d++ = *s++;
    *d++ = '\0';
  }

  return t;
}

static void mbtable_destroy(const struct ConfigSet *cs, void *var,
                            const struct VariableDef *vdef)
{
  if (!cs || !var || !vdef)
    return; /* LCOV_EXCL_LINE */

  struct MbTable **m = (struct MbTable **) var;
  if (!*m)
    return;

  mbtable_free(m);
}

static int mbtable_string_set(const struct ConfigSet *cs, void *var,
                              const struct VariableDef *vdef, const char *value,
                              struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  if (value && (value[0] == '\0'))
    value = NULL;

  struct MbTable *table = mbtable_parse(value);

  if (vdef->validator)
  {
    int rv = vdef->validator(cs, vdef, (intptr_t) table, err);

    if (CSR_RESULT(rv) != CSR_SUCCESS)
    {
      mbtable_free(&table);
      return rv | CSR_INV_VALIDATOR;
    }
  }

  mbtable_destroy(cs, var, vdef);

  int result = CSR_SUCCESS;
  if (!table)
    result |= CSR_SUC_EMPTY;

  *(struct MbTable **) var = table;
  return result;
}

static int mbtable_string_get(const struct ConfigSet *cs, void *var,
                              const struct VariableDef *vdef, struct Buffer *result)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct MbTable *table = *(struct MbTable **) var;
  if (!table || !table->orig_str)
    return CSR_SUCCESS | CSR_SUC_EMPTY; /* empty string */

  mutt_buffer_addstr(result, table->orig_str);
  return CSR_SUCCESS;
}

static struct MbTable *mbtable_dup(struct MbTable *table)
{
  if (!table)
    return NULL; /* LCOV_EXCL_LINE */

  struct MbTable *m = safe_calloc(1, sizeof(*m));
  m->orig_str = safe_strdup(table->orig_str);
  return m;
}

static int mbtable_native_set(const struct ConfigSet *cs, void *var,
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

  mbtable_free(var);

  struct MbTable *table = mbtable_dup((struct MbTable *) value);

  int result = CSR_SUCCESS;
  if (!table)
    result |= CSR_SUC_EMPTY;

  *(struct MbTable **) var = table;
  return result;
}

static intptr_t mbtable_native_get(const struct ConfigSet *cs, void *var,
                                   const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  struct MbTable *table = *(struct MbTable **) var;

  return (intptr_t) table;
}

static int mbtable_reset(const struct ConfigSet *cs, void *var,
                         const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  mbtable_destroy(cs, var, vdef);

  struct MbTable *table = NULL;
  const char *initial = (const char *) vdef->initial;

  if (initial)
    table = mbtable_parse(initial);

  int result = CSR_SUCCESS;
  if (!table)
    result |= CSR_SUC_EMPTY;

  *(struct MbTable **) var = table;
  return CSR_SUCCESS;
}

void mbtable_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_mbtable = {
    "mbtable",          mbtable_string_set, mbtable_string_get,
    mbtable_native_set, mbtable_native_get, mbtable_reset,
    mbtable_destroy,
  };
  cs_register_type(cs, DT_MBTABLE, &cst_mbtable);
}

struct MbTable *mbtable_create(const char *str)
{
  struct MbTable *m = safe_calloc(1, sizeof(*m));
  m->orig_str = safe_strdup(str);
  return m;
}

void mbtable_free(struct MbTable **table)
{
  if (!table || !*table)
    return; /* LCOV_EXCL_LINE */

  FREE(&(*table)->orig_str);
  FREE(&(*table)->chars);
  FREE(&(*table)->segmented_str);
  FREE(table);
}
