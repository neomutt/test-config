/**
 * @file
 * Test code for the Magic object
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
#include "mutt/buffer.h"
#include "mutt/memory.h"
#include "mutt/string2.h"
#include "config/account.h"
#include "config/magic.h"
#include "config/set.h"
#include "config/types.h"
#include "test/common.h"

static short VarApple;
static short VarBanana;
static short VarCherry;
static short VarDamson;
static short VarElderberry;
static short VarFig;
static short VarGuava;
static short VarHawthorn;
static short VarIlama;
static short VarJackfruit;
static short VarKumquat;

// clang-format off
static struct ConfigDef Vars[] = {
  { "Apple",      DT_MAGIC, 0, &VarApple,      1, NULL              }, /* test_initial_values() */
  { "Banana",     DT_MAGIC, 0, &VarBanana,     3, NULL              },
  { "Cherry",     DT_MAGIC, 0, &VarCherry,     1, NULL              }, /* test_string_set */
  { "Damson",     DT_MAGIC, 0, &VarDamson,     1, NULL              }, /* test_string_get */
  { "Elderberry", DT_MAGIC, 0, &VarElderberry, 1, NULL              }, /* test_native_set */
  { "Fig",        DT_MAGIC, 0, &VarFig,        1, NULL              }, /* test_native_get */
  { "Guava",      DT_MAGIC, 0, &VarGuava,      1, NULL              }, /* test_reset */
  { "Hawthorn",   DT_MAGIC, 0, &VarHawthorn,   1, validator_succeed }, /* test_validator */
  { "Ilama",      DT_MAGIC, 0, &VarIlama,      1, validator_warn    },
  { "Jackfruit",  DT_MAGIC, 0, &VarJackfruit,  1, validator_fail    },
  { "Kumquat",    DT_MAGIC, 0, &VarKumquat,    1, NULL              }, /* test_inherit */
  { NULL },
};
// clang-format on

static bool test_initial_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  printf("Apple = %d\n", VarApple);
  printf("Banana = %d\n", VarBanana);

  if ((VarApple != 1) || (VarBanana != 3))
  {
    printf("Error: initial values were wrong\n");
    return false;
  }

  cs_str_string_set(cs, "Apple", "MMDF", err);
  cs_str_string_set(cs, "Banana", "Maildir", err);

  struct Buffer value;
  mutt_buffer_init(&value);
  value.data = mutt_mem_calloc(1, STRING);
  value.dsize = STRING;
  mutt_buffer_reset(&value);

  int rc;

  mutt_buffer_reset(&value);
  rc = cs_str_initial_get(cs, "Apple", &value);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  if (mutt_str_strcmp(value.data, "mbox") != 0)
  {
    printf("Apple's initial value is wrong: '%s'\n", value.data);
    FREE(&value.data);
    return false;
  }
  printf("Apple = %d\n", VarApple);
  printf("Apple's initial value is %s\n", value.data);

  mutt_buffer_reset(&value);
  rc = cs_str_initial_get(cs, "Banana", &value);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  if (mutt_str_strcmp(value.data, "MH") != 0)
  {
    printf("Banana's initial value is wrong: %s\n", value.data);
    FREE(&value.data);
    return false;
  }
  printf("Banana = %d\n", VarBanana);
  printf("Banana's initial value is %s\n", NONULL(value.data));

  FREE(&value.data);
  return true;
}

static bool test_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "mbox", "mmdf", "mh", "maildir" };
  const char *invalid[] = {
    "mbox2", "mm", "", NULL,
  };
  char *name = "Cherry";

  int rc;
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    VarCherry = ((i + 1) % 4) + 1;

    mutt_buffer_reset(err);
    rc = cs_str_string_set(cs, name, valid[i], err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      printf("%s\n", err->data);
      return false;
    }

    if (VarCherry == (((i + 1) % 4) + 1))
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = %d, set by '%s'\n", name, VarCherry, valid[i]);
  }

  for (unsigned int i = 0; i < mutt_array_size(invalid); i++)
  {
    mutt_buffer_reset(err);
    rc = cs_str_string_set(cs, name, invalid[i], err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      printf("Expected error: %s\n", err->data);
    }
    else
    {
      printf("%s = %d, set by '%s'\n", name, VarCherry, invalid[i]);
      printf("This test should have failed\n");
      return false;
    }
  }

  return true;
}

static bool test_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Damson";

  int valid[] = { MUTT_MBOX, MUTT_MMDF, MUTT_MH, MUTT_MAILDIR };

  int rc;
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    VarDamson = valid[i];
    mutt_buffer_reset(err);
    rc = cs_str_string_get(cs, name, err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      printf("Get failed: %s\n", err->data);
      return false;
    }
    printf("%s = %d, %s\n", name, VarDamson, err->data);
  }

  VarDamson = 5;
  mutt_buffer_reset(err);
  printf("Expect error for next test\n");
  rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    return false;
  }

  return true;
}

static bool test_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Elderberry";
  short value = MUTT_MAILDIR;

  VarElderberry = MUTT_MBOX;
  mutt_buffer_reset(err);
  int rc = cs_str_native_set(cs, name, value, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    return false;
  }

  if (VarElderberry != value)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("%s = %d, set to '%d'\n", name, VarElderberry, value);

  int invalid[] = { 0, 5 };
  for (unsigned int i = 0; i < mutt_array_size(invalid); i++)
  {
    VarElderberry = MUTT_MBOX;
    mutt_buffer_reset(err);
    rc = cs_str_native_set(cs, name, invalid[i], err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      printf("Expected error: %s\n", err->data);
    }
    else
    {
      printf("%s = %d, set by '%d'\n", name, VarElderberry, invalid[i]);
      printf("This test should have failed\n");
      return false;
    }
  }

  return true;
}

static bool test_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Fig";

  VarFig = MUTT_MAILDIR;
  mutt_buffer_reset(err);
  intptr_t value = cs_str_native_get(cs, name, err);
  if (value == INT_MIN)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = %ld\n", name, value);

  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Guava";
  VarGuava = MUTT_MAILDIR;
  mutt_buffer_reset(err);

  int rc = cs_str_reset(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    return false;
  }

  if (VarGuava == MUTT_MAILDIR)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("Reset: %s = %d\n", name, VarGuava);

  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Hawthorn";
  VarHawthorn = MUTT_MBOX;
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, name, "maildir", err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %d\n", name, VarHawthorn);

  VarHawthorn = MUTT_MBOX;
  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, MUTT_MAILDIR, err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("Native: %s = %d\n", name, VarHawthorn);

  name = "Ilama";
  VarIlama = MUTT_MBOX;
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "maildir", err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %d\n", name, VarIlama);

  VarIlama = MUTT_MBOX;
  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, MUTT_MAILDIR, err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("Native: %s = %d\n", name, VarIlama);

  name = "Jackfruit";
  VarJackfruit = MUTT_MBOX;
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "maildir", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %d\n", name, VarJackfruit);

  VarJackfruit = MUTT_MBOX;
  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, MUTT_MAILDIR, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("Native: %s = %d\n", name, VarJackfruit);

  return true;
}

static void dump_native(struct ConfigSet *cs, const char *parent, const char *child)
{
  intptr_t pval = cs_str_native_get(cs, parent, NULL);
  intptr_t cval = cs_str_native_get(cs, child, NULL);

  printf("%15s = %ld\n", parent, pval);
  printf("%15s = %ld\n", child, cval);
}

static bool test_inherit(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  bool result = false;

  const char *account = "fruit";
  const char *parent = "Kumquat";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarStr[] = {
    parent, NULL,
  };

  struct Account *ac = ac_create(cs, account, AccountVarStr);

  // set parent
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, parent, "maildir", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, child, "mh", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // reset child
  mutt_buffer_reset(err);
  rc = cs_str_reset(cs, child, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // reset parent
  mutt_buffer_reset(err);
  rc = cs_str_reset(cs, parent, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  result = true;
ti_out:
  ac_free(cs, &ac);
  return result;
}

bool magic_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = mutt_mem_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_create(30);

  magic_init(cs);
  if (!cs_register_variables(cs, Vars, 0))
    return false;

  cs_add_listener(cs, log_listener);

  set_list(cs);

  if (!test_initial_values(cs, &err))
    return false;
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
  if (!test_validator(cs, &err))
    return false;
  if (!test_inherit(cs, &err))
    return false;

  cs_free(&cs);
  FREE(&err.data);

  return true;
}
