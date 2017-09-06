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

/**
 * @page config-mbtable Some mbtable
 *
 * LONG mbtable
 *
 * | Function           | Description
 * | :----------------- | :-----------------------------------------------
 * | mbtable_create     | Create an MbTable from a string
 * | mbtable_destroy    | Destroy an MbTable object
 * | mbtable_dup        | Create a copy of an MbTable object
 * | mbtable_free       | Free an MbTable object
 * | mbtable_init       | Register the MbTable config type
 * | mbtable_native_get | Get an MbTable object from a MbTable config item
 * | mbtable_native_set | Set a MbTable config item by MbTable object
 * | mbtable_parse      | Parse a multibyte string into a table
 * | mbtable_reset      | Reset an MbTable to its initial value
 * | mbtable_string_get | Get a MbTable as a string
 * | mbtable_string_set | Set a MbTable by string
 */

#include "config.h"
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include "config/mbtable.h"
#include "lib/buffer.h"
#include "lib/debug.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "set.h"
#include "types.h"

/**
 * mbtable_parse - Parse a multibyte string into a table
 * @param s String of multibyte characters
 * @retval ptr New MbTable object
 */
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
   * strings (Tochars and StatusChars), the overhead is not great. */
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

/**
 * mbtable_destroy - Destroy an MbTable object
 * @param cs   Config items
 * @param var  Variable to destroy
 * @param cdef Variable definition
 */
static void mbtable_destroy(const struct ConfigSet *cs, void *var,
                            const struct ConfigDef *cdef)
{
  if (!cs || !var || !cdef)
    return; /* LCOV_EXCL_LINE */

  struct MbTable **m = (struct MbTable **) var;
  if (!*m)
    return;

  mbtable_free(m);
}

/**
 * mbtable_string_set - Set a MbTable by string
 * @param cs    Config items
 * @param var   Variable to set
 * @param cdef  Variable definition
 * @param value Value to set
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int mbtable_string_set(const struct ConfigSet *cs, void *var,
                              const struct ConfigDef *cdef, const char *value,
                              struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  if (value && (value[0] == '\0'))
    value = NULL;

  struct MbTable *table = mbtable_parse(value);

  if (cdef->validator)
  {
    int rv = cdef->validator(cs, cdef, (intptr_t) table, err);

    if (CSR_RESULT(rv) != CSR_SUCCESS)
    {
      mbtable_free(&table);
      return rv | CSR_INV_VALIDATOR;
    }
  }

  mbtable_destroy(cs, var, cdef);

  int result = CSR_SUCCESS;
  if (!table)
    result |= CSR_SUC_EMPTY;

  *(struct MbTable **) var = table;
  return result;
}

/**
 * mbtable_string_get - Get a MbTable as a string
 * @param cs     Config items
 * @param var    Variable to get
 * @param cdef   Variable definition
 * @param result Buffer for results or error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int mbtable_string_get(const struct ConfigSet *cs, void *var,
                              const struct ConfigDef *cdef, struct Buffer *result)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct MbTable *table = *(struct MbTable **) var;
  if (!table || !table->orig_str)
    return CSR_SUCCESS | CSR_SUC_EMPTY; /* empty string */

  mutt_buffer_addstr(result, table->orig_str);
  return CSR_SUCCESS;
}

/**
 * mbtable_dup - Create a copy of an MbTable object
 * @param table MbTable to duplicate
 * @retval ptr New MbTable object
 */
static struct MbTable *mbtable_dup(struct MbTable *table)
{
  if (!table)
    return NULL; /* LCOV_EXCL_LINE */

  struct MbTable *m = safe_calloc(1, sizeof(*m));
  m->orig_str = safe_strdup(table->orig_str);
  return m;
}

/**
 * mbtable_native_set - Set a MbTable config item by MbTable object
 * @param cs    Config items
 * @param var   Variable to set
 * @param cdef  Variable definition
 * @param value MbTable pointer
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int mbtable_native_set(const struct ConfigSet *cs, void *var,
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

  mbtable_free(var);

  struct MbTable *table = mbtable_dup((struct MbTable *) value);

  int result = CSR_SUCCESS;
  if (!table)
    result |= CSR_SUC_EMPTY;

  *(struct MbTable **) var = table;
  return result;
}

/**
 * mbtable_native_get - Get an MbTable object from a MbTable config item
 * @param cs   Config items
 * @param var  Variable to get
 * @param cdef Variable definition
 * @param err  Buffer for error messages
 * @retval intptr_t MbTable pointer
 */
static intptr_t mbtable_native_get(const struct ConfigSet *cs, void *var,
                                   const struct ConfigDef *cdef, struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  struct MbTable *table = *(struct MbTable **) var;

  return (intptr_t) table;
}

/**
 * mbtable_reset - Reset an MbTable to its initial value
 * @param cs   Config items
 * @param var  Variable to reset
 * @param cdef Variable definition
 * @param err  Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int mbtable_reset(const struct ConfigSet *cs, void *var,
                         const struct ConfigDef *cdef, struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  mbtable_destroy(cs, var, cdef);

  struct MbTable *table = NULL;
  const char *initial = (const char *) cdef->initial;

  if (initial)
    table = mbtable_parse(initial);

  int result = CSR_SUCCESS;
  if (!table)
    result |= CSR_SUC_EMPTY;

  *(struct MbTable **) var = table;
  return CSR_SUCCESS;
}

/**
 * mbtable_init - Register the MbTable config type
 * @param cs Config items
 */
void mbtable_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_mbtable = {
    "mbtable",          mbtable_string_set, mbtable_string_get,
    mbtable_native_set, mbtable_native_get, mbtable_reset,
    mbtable_destroy,
  };
  cs_register_type(cs, DT_MBTABLE, &cst_mbtable);
}

/**
 * mbtable_create - Create an MbTable from a string
 * @param str String of multibyte characters
 * @retval ptr New MbTable object
 */
struct MbTable *mbtable_create(const char *str)
{
  struct MbTable *m = safe_calloc(1, sizeof(*m));
  m->orig_str = safe_strdup(str);
  return m;
}

/**
 * mbtable_free - Free an MbTable object
 * @param table MbTable to free
 */
void mbtable_free(struct MbTable **table)
{
  if (!table || !*table)
    return; /* LCOV_EXCL_LINE */

  FREE(&(*table)->orig_str);
  FREE(&(*table)->chars);
  FREE(&(*table)->segmented_str);
  FREE(table);
}
