/**
 * @file
 * Test code for the String object
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
#include "mutt/buffer.h"
#include "mutt/memory.h"
#include "mutt/string2.h"
#include "config/account.h"
#include "config/set.h"
#include "config/string3.h"
#include "config/types.h"
#include "test/common.h"

static char *VarApple;
static char *VarBanana;
static char *VarCherry;
static char *VarDamson;
static char *VarElderberry;
static char *VarFig;
static char *VarGuava;
static char *VarHawthorn;
static char *VarIlama;
static char *VarJackfruit;
static char *VarKumquat;
static char *VarLemon;
static char *VarMango;
static char *VarNectarine;
static char *VarOlive;
static char *VarPapaya;
static char *VarQuince;
static char *VarRaspberry;
static char *VarStrawberry;

// clang-format off
static struct ConfigDef Vars[] = {
  { "Apple",      DT_STRING,              0, &VarApple,      IP "apple",      NULL              }, /* test_initial_values() */
  { "Banana",     DT_STRING,              0, &VarBanana,     IP "banana",     NULL              },
  { "Cherry",     DT_STRING,              0, &VarCherry,     IP "cherry",     NULL              },
  { "Damson",     DT_STRING,              0, &VarDamson,     0,               NULL              }, /* test_string_set */
  { "Elderberry", DT_STRING,              0, &VarElderberry, IP "elderberry", NULL              },
  { "Raspberry",  DT_STRING|DT_NOT_EMPTY, 0, &VarRaspberry,  IP "raspberry",  NULL              },
  { "Fig",        DT_STRING,              0, &VarFig,        0,               NULL              }, /* test_string_get */
  { "Guava",      DT_STRING,              0, &VarGuava,      IP "guava",      NULL              },
  { "Hawthorn",   DT_STRING,              0, &VarHawthorn,   0,               NULL              },
  { "Ilama",      DT_STRING,              0, &VarIlama,      0,               NULL              }, /* test_native_set */
  { "Jackfruit",  DT_STRING,              0, &VarJackfruit,  IP "jackfruit",  NULL              },
  { "Strawberry", DT_STRING|DT_NOT_EMPTY, 0, &VarStrawberry, IP "strawberry", NULL              },
  { "Kumquat",    DT_STRING,              0, &VarKumquat,    0,               NULL              }, /* test_native_get */
  { "Lemon",      DT_STRING,              0, &VarLemon,      IP "lemon",      NULL              }, /* test_reset */
  { "Quince",     DT_STRING,              0, &VarQuince,     IP "quince",     validator_fail    },
  { "Mango",      DT_STRING,              0, &VarMango,      IP "mango",      validator_succeed }, /* test_validator */
  { "Nectarine",  DT_STRING,              0, &VarNectarine,  IP "nectarine",  validator_warn    },
  { "Olive",      DT_STRING,              0, &VarOlive,      IP "olive",      validator_fail    },
  { "Papaya",     DT_STRING,              0, &VarPapaya,     0,               NULL              }, /* test_inherit */
  { NULL },
};
// clang-format on

static bool test_initial_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  printf("Apple = %s\n", VarApple);
  printf("Banana = %s\n", VarBanana);

  if ((mutt_str_strcmp(VarApple, "apple") != 0) ||
      (mutt_str_strcmp(VarBanana, "banana") != 0))
  {
    printf("Error: initial values were wrong\n");
    return false;
  }

  cs_str_string_set(cs, "Apple", "car", err);
  cs_str_string_set(cs, "Banana", NULL, err);

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

  if (mutt_str_strcmp(value.data, "apple") != 0)
  {
    printf("Apple's initial value is wrong: '%s'\n", value.data);
    FREE(&value.data);
    return false;
  }
  printf("Apple = '%s'\n", VarApple);
  printf("Apple's initial value is '%s'\n", value.data);

  mutt_buffer_reset(&value);
  rc = cs_str_initial_get(cs, "Banana", &value);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  if (mutt_str_strcmp(value.data, "banana") != 0)
  {
    printf("Banana's initial value is wrong: '%s'\n", value.data);
    FREE(&value.data);
    return false;
  }
  printf("Banana = '%s'\n", VarBanana);
  printf("Banana's initial value is '%s'\n", NONULL(value.data));

  mutt_buffer_reset(&value);
  rc = cs_str_initial_set(cs, "Cherry", "train", &value);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  mutt_buffer_reset(&value);
  rc = cs_str_initial_set(cs, "Cherry", "plane", &value);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  mutt_buffer_reset(&value);
  rc = cs_str_initial_get(cs, "Cherry", &value);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  printf("Cherry = '%s'\n", VarCherry);
  printf("Cherry's initial value is '%s'\n", NONULL(value.data));

  FREE(&value.data);
  return true;
}

static bool test_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "hello", "world", "", NULL };
  char *name = "Damson";

  int rc;
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    rc = cs_str_string_set(cs, name, valid[i], err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      printf("%s\n", err->data);
      return false;
    }

    if (rc & CSR_SUC_NO_CHANGE)
    {
      printf("Value of %s wasn't changed\n", name);
      continue;
    }

    if (mutt_str_strcmp(VarDamson, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(VarDamson), NONULL(valid[i]));
  }

  name = "Raspberry";
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }

  name = "Elderberry";
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    rc = cs_str_string_set(cs, name, valid[i], err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      printf("%s\n", err->data);
      return false;
    }

    if (rc & CSR_SUC_NO_CHANGE)
    {
      printf("Value of %s wasn't changed\n", name);
      continue;
    }

    if (mutt_str_strcmp(VarElderberry, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(VarElderberry), NONULL(valid[i]));
  }

  return true;
}

static bool test_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Fig";

  mutt_buffer_reset(err);
  int rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, NONULL(VarFig), err->data);

  name = "Guava";
  mutt_buffer_reset(err);
  rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, NONULL(VarGuava), err->data);

  name = "Hawthorn";
  rc = cs_str_string_set(cs, name, "hawthorn", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;

  mutt_buffer_reset(err);
  rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, NONULL(VarHawthorn), err->data);

  return true;
}

static bool test_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "hello", "world", "", NULL };
  char *name = "Ilama";

  int rc;
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    rc = cs_str_native_set(cs, name, (intptr_t) valid[i], err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      printf("%s\n", err->data);
      return false;
    }

    if (rc & CSR_SUC_NO_CHANGE)
    {
      printf("Value of %s wasn't changed\n", name);
      continue;
    }

    if (mutt_str_strcmp(VarIlama, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(VarIlama), NONULL(valid[i]));
  }

  name = "Strawberry";
  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, (intptr_t) "", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }

  name = "Jackfruit";
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    rc = cs_str_native_set(cs, name, (intptr_t) valid[i], err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      printf("%s\n", err->data);
      return false;
    }

    if (rc & CSR_SUC_NO_CHANGE)
    {
      printf("Value of %s wasn't changed\n", name);
      continue;
    }

    if (mutt_str_strcmp(VarJackfruit, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(VarJackfruit), NONULL(valid[i]));
  }

  return true;
}

static bool test_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Kumquat";

  int rc = cs_str_string_set(cs, name, "kumquat", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;

  mutt_buffer_reset(err);
  intptr_t value = cs_str_native_get(cs, name, err);
  if (mutt_str_strcmp(VarKumquat, (char *) value) != 0)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, VarKumquat, (char *) value);

  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Lemon";
  mutt_buffer_reset(err);

  printf("Initial: %s = '%s'\n", name, VarLemon);
  int rc = cs_str_string_set(cs, name, "hello", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;
  printf("Set: %s = '%s'\n", name, VarLemon);

  rc = cs_str_reset(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    return false;
  }

  if (mutt_str_strcmp(VarLemon, "lemon") != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("Reset: %s = '%s'\n", name, VarLemon);

  name = "Quince";
  mutt_buffer_reset(err);

  printf("Initial: %s = '%s'\n", name, VarQuince);
  dont_fail = true;
  rc = cs_str_string_set(cs, name, "hello", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;
  printf("Set: %s = '%s'\n", name, VarQuince);
  dont_fail = false;

  rc = cs_str_reset(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }

  if (mutt_str_strcmp(VarQuince, "hello") != 0)
  {
    printf("Value of %s changed\n", name);
    return false;
  }

  printf("Reset: %s = '%s'\n", name, VarQuince);

  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Mango";
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, name, "hello", err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %s\n", name, VarMango);

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP "world", err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("Native: %s = %s\n", name, VarMango);

  name = "Nectarine";
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "hello", err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %s\n", name, VarNectarine);

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP "world", err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("Native: %s = %s\n", name, VarNectarine);

  name = "Olive";
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "hello", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %s\n", name, VarOlive);

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP "world", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("Native: %s = %s\n", name, VarOlive);

  return true;
}

static void dump_native(struct ConfigSet *cs, const char *parent, const char *child)
{
  intptr_t pval = cs_str_native_get(cs, parent, NULL);
  intptr_t cval = cs_str_native_get(cs, child, NULL);

  printf("%15s = %s\n", parent, (char *) pval);
  printf("%15s = %s\n", child, (char *) cval);
}

static bool test_inherit(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  bool result = false;

  const char *account = "fruit";
  const char *parent = "Papaya";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarStr[] = {
    parent, NULL,
  };

  struct Account *ac = ac_create(cs, account, AccountVarStr);

  // set parent
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, parent, "hello", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, child, "world", err);
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

bool string_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = mutt_mem_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_create(30);

  string_init(cs);
  dont_fail = true;
  if (!cs_register_variables(cs, Vars, 0))
    return false;
  dont_fail = false;

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
