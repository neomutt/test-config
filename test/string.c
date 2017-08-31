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
#include "mutt_options.h"
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

// clang-format off
static struct VariableDef Vars[] = {
  { "Apple",      DT_STRING, &VarApple,      IP "apple",     NULL              }, /* test_initial_values() */
  { "Banana",     DT_STRING, &VarBanana,     IP "banana",    NULL              },
  { "Cherry",     DT_STRING, &VarCherry,     0,              NULL              }, /* test_string_set */
  { "Damson",     DT_STRING, &VarDamson,     IP "damson",    NULL              },
  { "Elderberry", DT_STRING, &VarElderberry, 0,              NULL              }, /* test_string_get */
  { "Fig",        DT_STRING, &VarFig,        IP "fig",       NULL              },
  { "Guava",      DT_STRING, &VarGuava,      0,              NULL              },
  { "Hawthorn",   DT_STRING, &VarHawthorn,   0,              NULL              }, /* test_native_set */
  { "Ilama",      DT_STRING, &VarIlama,      IP "ilama",     NULL              },
  { "Jackfruit",  DT_STRING, &VarJackfruit,  0,              NULL              }, /* test_native_get */
  { "Kumquat",    DT_STRING, &VarKumquat,    IP "kumquat",   NULL              }, /* test_reset */
  { "Lemon",      DT_STRING, &VarLemon,      IP "lemon",     validator_succeed }, /* test_validator */
  { "Mango",      DT_STRING, &VarMango,      IP "mango",     validator_warn    },
  { "Nectarine",  DT_STRING, &VarNectarine,  IP "nectarine", validator_fail    },
  { "Olive",      DT_STRING, &VarOlive,      0,              NULL              }, /* test_inherit */
  { NULL },
};
// clang-format on

static bool test_initial_values_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  printf("Apple = %s\n", VarApple);
  printf("Banana = %s\n", VarBanana);

  return ((mutt_strcmp(VarApple, "apple") == 0) &&
          (mutt_strcmp(VarBanana, "banana") == 0));
}

static bool test_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "hello", "world", "", NULL };
  char *name = "Cherry";

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

    if (mutt_strcmp(VarCherry, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(VarCherry), NONULL(valid[i]));
  }

  name = "Damson";
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    rc = cs_str_string_set(cs, name, valid[i], err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      printf("%s\n", err->data);
      return false;
    }

    if (mutt_strcmp(VarDamson, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(VarDamson), NONULL(valid[i]));
  }

  return true;
}

static bool test_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Elderberry";

  mutt_buffer_reset(err);
  int rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, NONULL(VarElderberry), err->data);

  name = "Fig";
  mutt_buffer_reset(err);
  rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, NONULL(VarFig), err->data);

  name = "Guava";
  rc = cs_str_string_set(cs, name, "guava", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;

  mutt_buffer_reset(err);
  rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, NONULL(VarGuava), err->data);

  return true;
}

static bool test_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "hello", "world", "", NULL };
  char *name = "Hawthorn";

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

    if (mutt_strcmp(VarHawthorn, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(VarHawthorn), NONULL(valid[i]));
  }

  name = "Ilama";
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    rc = cs_str_native_set(cs, name, (intptr_t) valid[i], err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      printf("%s\n", err->data);
      return false;
    }

    if (mutt_strcmp(VarIlama, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(VarIlama), NONULL(valid[i]));
  }

  return true;
}

static bool test_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Jackfruit";

  int rc = cs_str_string_set(cs, name, "jackfruit", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;

  mutt_buffer_reset(err);
  intptr_t value = cs_str_native_get(cs, name, err);
  if (mutt_strcmp(VarJackfruit, (char *) value) != 0)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, VarJackfruit, (char *) value);

  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Kumquat";
  mutt_buffer_reset(err);

  printf("Initial: %s = '%s'\n", name, VarKumquat);
  int rc = cs_str_string_set(cs, name, "hello", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;
  printf("Set: %s = '%s'\n", name, VarKumquat);

  rc = cs_reset_variable(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    return false;
  }

  if (mutt_strcmp(VarKumquat, "kumquat") != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("Reset: %s = '%s'\n", name, VarKumquat);

  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Lemon";
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
  printf("String: %s = %s\n", name, VarLemon);

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
  printf("Native: %s = %s\n", name, VarLemon);

  name = "Mango";
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
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %s\n", name, VarNectarine);

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
  printf("Native: %s = %s\n", name, VarNectarine);

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
  const char *parent = "Olive";
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
  rc = cs_reset_variable(cs, child, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // reset parent
  mutt_buffer_reset(err);
  rc = cs_reset_variable(cs, parent, err);
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
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_create(30);

  string_init(cs);
  if (!cs_register_variables(cs, Vars))
    return false;

  cs_add_listener(cs, log_listener);

  set_list(cs);

  if (!test_initial_values_values(cs, &err))
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
