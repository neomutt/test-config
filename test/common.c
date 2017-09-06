/**
 * @file
 * Shared Testing Code
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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "config/inheritance.h"
#include "config/set.h"
#include "config/types.h"
#include "lib/buffer.h"
#include "lib/hash.h"
#include "lib/memory.h"
#include "lib/string2.h"

struct HashElem;

const char *line = "----------------------------------------"
                   "----------------------------------------";

int validator_fail(const struct ConfigSet *cs, const struct ConfigDef *cdef,
                   intptr_t value, struct Buffer *result)
{
  if (value > 1000000)
    mutt_buffer_printf(result, "%s: %s, (ptr)", __func__, cdef->name);
  else
    mutt_buffer_printf(result, "%s: %s, %ld", __func__, cdef->name, value);
  return CSR_ERR_INVALID;
}

int validator_warn(const struct ConfigSet *cs, const struct ConfigDef *cdef,
                   intptr_t value, struct Buffer *result)
{
  if (value > 1000000)
    mutt_buffer_printf(result, "%s: %s, (ptr)", __func__, cdef->name);
  else
    mutt_buffer_printf(result, "%s: %s, %ld", __func__, cdef->name, value);
  return CSR_SUCCESS | CSR_SUC_WARNING;
}

int validator_succeed(const struct ConfigSet *cs, const struct ConfigDef *cdef,
                      intptr_t value, struct Buffer *result)
{
  if (value > 1000000)
    mutt_buffer_printf(result, "%s: %s, (ptr)", __func__, cdef->name);
  else
    mutt_buffer_printf(result, "%s: %s, %ld", __func__, cdef->name, value);
  return CSR_SUCCESS;
}

void log_line(const char *fn)
{
  int len = 54 - mutt_strlen(fn);
  printf("---- %s %.*s\n", fn, len, line);
}

bool log_listener(const struct ConfigSet *cs, struct HashElem *he,
                  const char *name, enum ConfigEvent ev)
{
  struct Buffer result;
  mutt_buffer_init(&result);
  result.data = safe_calloc(1, STRING);
  result.dsize = STRING;

  const char *events[] = { "set", "reset" };

  mutt_buffer_reset(&result);
  cs_he_native_get(cs, he, &result);

  printf("Event: %s has been %s to '%s'\n", name, events[ev], result.data);

  FREE(&result.data);
  return true;
}

void set_list(const struct ConfigSet *cs)
{
  log_line(__func__);
  cs_dump_set(cs);
  // hash_dump(cs->hash);
}

void hash_dump(struct Hash *table)
{
  if (!table)
    return;

  struct HashElem *he = NULL;

  for (int i = 0; i < table->nelem; i++)
  {
    he = table->table[i];
    if (!he)
      continue;

    if (he->type == DT_SYNONYM)
      continue;

    printf("%03d ", i);
    for (; he; he = he->next)
    {
      if (he->type & DT_INHERITED)
      {
        struct Inheritance *inh = he->data;
        printf("\033[1;32m[%s]\033[m ", inh->name);
      }
      else
      {
        printf("%s ", *(char **) he->data);
      }
    }
    printf("\n");
  }
}

void cs_dump_set(const struct ConfigSet *cs)
{
  if (!cs)
    return;

  struct HashElem *he = NULL;
  struct HashWalkState state;
  memset(&state, 0, sizeof(state));

  struct Buffer result;
  mutt_buffer_init(&result);
  result.data = safe_calloc(1, STRING);
  result.dsize = STRING;

  while ((he = hash_walk(cs->hash, &state)))
  {
    if (he->type == DT_SYNONYM)
      continue;

    const char *name = NULL;

    if (he->type & DT_INHERITED)
    {
      struct Inheritance *i = he->data;
      he = i->parent;
      name = i->name;
    }
    else
    {
      name = he->key.strkey;
    }

    const struct ConfigSetType *cst = cs_get_type_def(cs, he->type);
    if (!cst)
    {
      printf("Unknown type: %d\n", he->type);
      continue;
    }

    mutt_buffer_reset(&result);
    printf("%s %s", cst->name, name);

    const struct ConfigDef *cdef = he->data;

    int rc = cst->string_get(cs, cdef->var, cdef, &result);
    if (CSR_RESULT(rc) == CSR_SUCCESS)
      printf(" = %s\n", result.data);
    else
      printf(": ERROR: %s\n", result.data);
  }

  FREE(&result.data);
}
