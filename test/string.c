/**
 * @file
 * Test code for the String object
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

#define TEST_NO_MAIN
#include "acutest.h"
#include "config.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "mutt/mutt.h"
#include "common.h"
#include "config/lib.h"
#include "account.h"

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
  { "Apple",      DT_STRING,              &VarApple,      IP "apple",      0, NULL              }, /* test_initial_values */
  { "Banana",     DT_STRING,              &VarBanana,     IP "banana",     0, NULL              },
  { "Cherry",     DT_STRING,              &VarCherry,     IP "cherry",     0, NULL              },
  { "Damson",     DT_STRING,              &VarDamson,     0,               0, NULL              }, /* test_string_set */
  { "Elderberry", DT_STRING,              &VarElderberry, IP "elderberry", 0, NULL              },
  { "Fig",        DT_STRING|DT_NOT_EMPTY, &VarFig,        IP "fig",        0, NULL              },
  { "Guava",      DT_STRING,              &VarGuava,      0,               0, NULL              }, /* test_string_get */
  { "Hawthorn",   DT_STRING,              &VarHawthorn,   IP "hawthorn",   0, NULL              },
  { "Ilama",      DT_STRING,              &VarIlama,      0,               0, NULL              },
  { "Jackfruit",  DT_STRING,              &VarJackfruit,  0,               0, NULL              }, /* test_native_set */
  { "Kumquat",    DT_STRING,              &VarKumquat,    IP "kumquat",    0, NULL              },
  { "Lemon",      DT_STRING|DT_NOT_EMPTY, &VarLemon,      IP "lemon",      0, NULL              },
  { "Mango",      DT_STRING,              &VarMango,      0,               0, NULL              }, /* test_native_get */
  { "Nectarine",  DT_STRING,              &VarNectarine,  IP "nectarine",  0, NULL              }, /* test_reset */
  { "Olive",      DT_STRING,              &VarOlive,      IP "olive",      0, validator_fail    },
  { "Papaya",     DT_STRING,              &VarPapaya,     IP "papaya",     0, validator_succeed }, /* test_validator */
  { "Quince",     DT_STRING,              &VarQuince,     IP "quince",     0, validator_warn    },
  { "Raspberry",  DT_STRING,              &VarRaspberry,  IP "raspberry",  0, validator_fail    },
  { "Strawberry", DT_STRING,              &VarStrawberry, 0,               0, NULL              }, /* test_inherit */
  { NULL },
};
// clang-format on

static bool test_initial_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  TEST_MSG("Apple = %s\n", VarApple);
  TEST_MSG("Banana = %s\n", VarBanana);

  if (!TEST_CHECK(mutt_str_strcmp(VarApple, "apple") == 0))
  {
    TEST_MSG("Error: initial values were wrong\n");
    return false;
  }

  if (!TEST_CHECK(mutt_str_strcmp(VarBanana, "banana") == 0))
  {
    TEST_MSG("Error: initial values were wrong\n");
    return false;
  }

  cs_str_string_set(cs, "Apple", "car", err);
  cs_str_string_set(cs, "Banana", NULL, err);

  struct Buffer value;
  mutt_buffer_init(&value);
  value.dsize = 256;
  value.data = mutt_mem_calloc(1, value.dsize);
  mutt_buffer_reset(&value);

  int rc;

  mutt_buffer_reset(&value);
  rc = cs_str_initial_get(cs, "Apple", &value);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  if (!TEST_CHECK(mutt_str_strcmp(value.data, "apple") == 0))
  {
    TEST_MSG("Apple's initial value is wrong: '%s'\n", value.data);
    FREE(&value.data);
    return false;
  }
  TEST_MSG("Apple = '%s'\n", VarApple);
  TEST_MSG("Apple's initial value is '%s'\n", value.data);

  mutt_buffer_reset(&value);
  rc = cs_str_initial_get(cs, "Banana", &value);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  if (!TEST_CHECK(mutt_str_strcmp(value.data, "banana") == 0))
  {
    TEST_MSG("Banana's initial value is wrong: '%s'\n", value.data);
    FREE(&value.data);
    return false;
  }
  TEST_MSG("Banana = '%s'\n", VarBanana);
  TEST_MSG("Banana's initial value is '%s'\n", NONULL(value.data));

  mutt_buffer_reset(&value);
  rc = cs_str_initial_set(cs, "Cherry", "train", &value);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  mutt_buffer_reset(&value);
  rc = cs_str_initial_set(cs, "Cherry", "plane", &value);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  mutt_buffer_reset(&value);
  rc = cs_str_initial_get(cs, "Cherry", &value);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  TEST_MSG("Cherry = '%s'\n", VarCherry);
  TEST_MSG("Cherry's initial value is '%s'\n", NONULL(value.data));

  FREE(&value.data);
  log_line(__func__);
  return true;
}

static bool test_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "hello", "world", "world", "", NULL };
  const char *name = "Damson";

  int rc;
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    rc = cs_str_string_set(cs, name, valid[i], err);
    if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    {
      TEST_MSG("%s\n", err->data);
      return false;
    }

    if (rc & CSR_SUC_NO_CHANGE)
    {
      TEST_MSG("Value of %s wasn't changed\n", name);
      continue;
    }

    if (!TEST_CHECK(mutt_str_strcmp(VarDamson, valid[i]) == 0))
    {
      TEST_MSG("Value of %s wasn't changed\n", name);
      return false;
    }
    TEST_MSG("%s = '%s', set by '%s'\n", name, NONULL(VarDamson), NONULL(valid[i]));
    short_line();
  }

  name = "Fig";
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "", err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    return false;
  }

  name = "Elderberry";
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    short_line();
    mutt_buffer_reset(err);
    rc = cs_str_string_set(cs, name, valid[i], err);
    if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    {
      TEST_MSG("%s\n", err->data);
      return false;
    }

    if (rc & CSR_SUC_NO_CHANGE)
    {
      TEST_MSG("Value of %s wasn't changed\n", name);
      continue;
    }

    if (!TEST_CHECK(mutt_str_strcmp(VarElderberry, valid[i]) == 0))
    {
      TEST_MSG("Value of %s wasn't changed\n", name);
      return false;
    }
    TEST_MSG("%s = '%s', set by '%s'\n", name, NONULL(VarElderberry), NONULL(valid[i]));
  }

  log_line(__func__);
  return true;
}

static bool test_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Guava";

  mutt_buffer_reset(err);
  int rc = cs_str_string_get(cs, name, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Get failed: %s\n", err->data);
    return false;
  }
  TEST_MSG("%s = '%s', '%s'\n", name, NONULL(VarGuava), err->data);

  name = "Hawthorn";
  mutt_buffer_reset(err);
  rc = cs_str_string_get(cs, name, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Get failed: %s\n", err->data);
    return false;
  }
  TEST_MSG("%s = '%s', '%s'\n", name, NONULL(VarHawthorn), err->data);

  name = "Ilama";
  rc = cs_str_string_set(cs, name, "ilama", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    return false;

  mutt_buffer_reset(err);
  rc = cs_str_string_get(cs, name, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Get failed: %s\n", err->data);
    return false;
  }
  TEST_MSG("%s = '%s', '%s'\n", name, NONULL(VarIlama), err->data);

  log_line(__func__);
  return true;
}

static bool test_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "hello", "world", "world", "", NULL };
  const char *name = "Jackfruit";

  int rc;
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    rc = cs_str_native_set(cs, name, (intptr_t) valid[i], err);
    if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    {
      TEST_MSG("%s\n", err->data);
      return false;
    }

    if (rc & CSR_SUC_NO_CHANGE)
    {
      TEST_MSG("Value of %s wasn't changed\n", name);
      continue;
    }

    if (!TEST_CHECK(mutt_str_strcmp(VarJackfruit, valid[i]) == 0))
    {
      TEST_MSG("Value of %s wasn't changed\n", name);
      return false;
    }
    TEST_MSG("%s = '%s', set by '%s'\n", name, NONULL(VarJackfruit), NONULL(valid[i]));
    short_line();
  }

  name = "Lemon";
  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, (intptr_t) "", err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    return false;
  }

  name = "Kumquat";
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    short_line();
    mutt_buffer_reset(err);
    rc = cs_str_native_set(cs, name, (intptr_t) valid[i], err);
    if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    {
      TEST_MSG("%s\n", err->data);
      return false;
    }

    if (rc & CSR_SUC_NO_CHANGE)
    {
      TEST_MSG("Value of %s wasn't changed\n", name);
      continue;
    }

    if (!TEST_CHECK(mutt_str_strcmp(VarKumquat, valid[i]) == 0))
    {
      TEST_MSG("Value of %s wasn't changed\n", name);
      return false;
    }
    TEST_MSG("%s = '%s', set by '%s'\n", name, NONULL(VarKumquat), NONULL(valid[i]));
  }

  log_line(__func__);
  return true;
}

static bool test_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Mango";

  int rc = cs_str_string_set(cs, name, "mango", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    return false;

  mutt_buffer_reset(err);
  intptr_t value = cs_str_native_get(cs, name, err);
  if (!TEST_CHECK(mutt_str_strcmp(VarMango, (char *) value) == 0))
  {
    TEST_MSG("Get failed: %s\n", err->data);
    return false;
  }
  TEST_MSG("%s = '%s', '%s'\n", name, VarMango, (char *) value);

  log_line(__func__);
  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *name = "Nectarine";
  mutt_buffer_reset(err);

  TEST_MSG("Initial: %s = '%s'\n", name, VarNectarine);
  int rc = cs_str_string_set(cs, name, "hello", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    return false;
  TEST_MSG("Set: %s = '%s'\n", name, VarNectarine);

  rc = cs_str_reset(cs, name, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
    return false;
  }

  if (!TEST_CHECK(mutt_str_strcmp(VarNectarine, "nectarine") == 0))
  {
    TEST_MSG("Value of %s wasn't changed\n", name);
    return false;
  }

  TEST_MSG("Reset: %s = '%s'\n", name, VarNectarine);

  rc = cs_str_reset(cs, name, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
    return false;
  }

  name = "Olive";
  mutt_buffer_reset(err);

  TEST_MSG("Initial: %s = '%s'\n", name, VarOlive);
  dont_fail = true;
  rc = cs_str_string_set(cs, name, "hello", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    return false;
  TEST_MSG("Set: %s = '%s'\n", name, VarOlive);
  dont_fail = false;

  rc = cs_str_reset(cs, name, err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    return false;
  }

  if (!TEST_CHECK(mutt_str_strcmp(VarOlive, "hello") == 0))
  {
    TEST_MSG("Value of %s changed\n", name);
    return false;
  }

  TEST_MSG("Reset: %s = '%s'\n", name, VarOlive);

  log_line(__func__);
  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *name = "Papaya";
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, name, "hello", err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    return false;
  }
  TEST_MSG("String: %s = %s\n", name, VarPapaya);

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP "world", err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    return false;
  }
  TEST_MSG("Native: %s = %s\n", name, VarPapaya);

  name = "Quince";
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "hello", err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    return false;
  }
  TEST_MSG("String: %s = %s\n", name, VarQuince);

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP "world", err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    return false;
  }
  TEST_MSG("Native: %s = %s\n", name, VarQuince);

  name = "Raspberry";
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "hello", err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    return false;
  }
  TEST_MSG("String: %s = %s\n", name, VarRaspberry);

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP "world", err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    return false;
  }
  TEST_MSG("Native: %s = %s\n", name, VarRaspberry);

  log_line(__func__);
  return true;
}

static void dump_native(struct ConfigSet *cs, const char *parent, const char *child)
{
  intptr_t pval = cs_str_native_get(cs, parent, NULL);
  intptr_t cval = cs_str_native_get(cs, child, NULL);

  TEST_MSG("%15s = %s\n", parent, (char *) pval);
  TEST_MSG("%15s = %s\n", child, (char *) cval);
}

static bool test_inherit(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  bool result = false;

  const char *account = "fruit";
  const char *parent = "Strawberry";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarStr[] = {
    parent,
    NULL,
  };

  struct Account *a = account_new(cs, NULL);
  account_add_config(a, cs, account, AccountVarStr);

  // set parent
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, parent, "hello", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, child, "world", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // reset child
  mutt_buffer_reset(err);
  rc = cs_str_reset(cs, child, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // reset parent
  mutt_buffer_reset(err);
  rc = cs_str_reset(cs, parent, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  log_line(__func__);
  result = true;
ti_out:
  account_free(&a);
  return result;
}

void config_string(void)
{
  struct Buffer err;
  mutt_buffer_init(&err);
  err.dsize = 256;
  err.data = mutt_mem_calloc(1, err.dsize);
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new(30);

  string_init(cs);
  dont_fail = true;
  if (!cs_register_variables(cs, Vars, 0))
    return;
  dont_fail = false;

  notify_observer_add(cs->notify, NT_CONFIG, 0, log_observer, 0);

  set_list(cs);

  TEST_CHECK(test_initial_values(cs, &err));
  TEST_CHECK(test_string_set(cs, &err));
  TEST_CHECK(test_string_get(cs, &err));
  TEST_CHECK(test_native_set(cs, &err));
  TEST_CHECK(test_native_get(cs, &err));
  TEST_CHECK(test_reset(cs, &err));
  TEST_CHECK(test_validator(cs, &err));
  TEST_CHECK(test_inherit(cs, &err));

  cs_free(&cs);
  FREE(&err.data);
}
