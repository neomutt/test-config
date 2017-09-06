/**
 * @file
 * Test code for Config Synonyms
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
#include <stdio.h>
#include "config/account.h"
#include "config/set.h"
#include "config/string3.h"
#include "config/types.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "test/common.h"

static char *VarCherry;
static char *VarApple;
static char *VarElderberry;
static char *VarGuava;
static char *VarIlama;

// clang-format off
static struct ConfigDef Vars[] = {
  { "Apple",      DT_STRING,  0, &VarApple,      0,               NULL },
  { "Banana",     DT_SYNONYM, 0, NULL,           IP "Apple",      NULL },
  { "Cherry",     DT_STRING,  0, &VarCherry,     IP "cherry",     NULL },
  { "Damson",     DT_SYNONYM, 0, NULL,           IP "Cherry",     NULL },
  { "Elderberry", DT_STRING,  0, &VarElderberry, 0,               NULL },
  { "Fig",        DT_SYNONYM, 0, NULL,           IP "Elderberry", NULL },
  { "Guava",      DT_STRING,  0, &VarGuava,      0,               NULL },
  { "Hawthorn",   DT_SYNONYM, 0, NULL,           IP "Guava",      NULL },
  { "Ilama",      DT_STRING,  0, &VarIlama,      IP "iguana",     NULL },
  { "Jackfruit",  DT_SYNONYM, 0, NULL,           IP "Ilama",      NULL },
  { NULL },
};

static struct ConfigDef Vars2[] = {
  { "Jackfruit",  DT_SYNONYM, 0, NULL,           IP "Broken",     NULL },
  { NULL },
};
// clang-format on

static bool test_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *name = "Banana";
  const char *value = "pudding";

  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, name, value, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    return false;
  }

  if (mutt_strcmp(VarApple, value) != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }
  printf("%s = %s, set by '%s'\n", name, NONULL(VarApple), value);

  return true;
}

static bool test_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Damson";

  mutt_buffer_reset(err);
  int rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, NONULL(VarCherry), err->data);

  return true;
}

static bool test_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Fig";
  char *value = "tree";

  mutt_buffer_reset(err);
  int rc = cs_str_native_set(cs, name, (intptr_t) value, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    return false;
  }

  if (mutt_strcmp(VarElderberry, value) != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }
  printf("%s = %s, set by '%s'\n", name, NONULL(VarElderberry), value);

  return true;
}

static bool test_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Hawthorn";

  int rc = cs_str_string_set(cs, name, "tree", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;

  mutt_buffer_reset(err);
  intptr_t value = cs_str_native_get(cs, name, err);
  if (mutt_strcmp(VarGuava, (const char *) value) != 0)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, VarGuava, (const char *) value);

  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Jackfruit";
  mutt_buffer_reset(err);

  printf("Initial: %s = '%s'\n", name, NONULL(VarIlama));
  int rc = cs_str_string_set(cs, name, "hello", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;
  printf("Set: %s = '%s'\n", name, VarIlama);

  mutt_buffer_reset(err);
  rc = cs_reset_variable(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    return false;
  }

  if (mutt_strcmp(VarIlama, "iguana") != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("Reset: %s = '%s'\n", name, VarIlama);

  return true;
}

bool synonym_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_create(30);

  string_init(cs);
  if (!cs_register_variables(cs, Vars))
    return false;

  if (!cs_register_variables(cs, Vars2))
  {
    printf("Expected error\n");
  }
  else
  {
    printf("Test should have failed\n");
    return false;
  }

  cs_add_listener(cs, log_listener);

  set_list(cs);

  if (!test_string_set(cs, &err))
    return false;
  if (!test_string_get(cs, &err))
    return false;
  if (!test_native_set(cs, &err))
    return false;
  if (!test_native_get(cs, &err))
    return false;
  if (!test_reset(cs, &err))
    return false;

  cs_free(&cs);
  FREE(&err.data);

  return true;
}
