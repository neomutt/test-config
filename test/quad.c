/**
 * @file
 * Test code for the Quad object
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
#include "config/bool.h"
#include "config/quad.h"
#include "config/set.h"
#include "config/types.h"
#include "test/common.h"

static char VarApple;
static char VarBanana;
static char VarCherry;
static char VarDamson;
static char VarElderberry;
static char VarFig;
static char VarGuava;
static char VarHawthorn;
static char VarIlama;
static char VarJackfruit;
static char VarKumquat;
static char VarLemon;
static char VarMango;
static char VarNectarine;

// clang-format off
static struct ConfigDef Vars[] = {
  { "Apple",      DT_QUAD, 0, &VarApple,      0, NULL              }, /* test_initial_values() */
  { "Banana",     DT_QUAD, 0, &VarBanana,     3, NULL              },
  { "Cherry",     DT_QUAD, 0, &VarCherry,     0, NULL              },
  { "Damson",     DT_QUAD, 0, &VarDamson,     0, NULL              }, /* test_string_set */
  { "Elderberry", DT_QUAD, 0, &VarElderberry, 0, NULL              }, /* test_string_get */
  { "Fig",        DT_QUAD, 0, &VarFig,        0, NULL              }, /* test_native_set */
  { "Guava",      DT_QUAD, 0, &VarGuava,      0, NULL              }, /* test_native_get */
  { "Hawthorn",   DT_QUAD, 0, &VarHawthorn,   0, NULL              }, /* test_reset */
  { "Ilama",      DT_QUAD, 0, &VarIlama,      0, validator_succeed }, /* test_validator */
  { "Jackfruit",  DT_QUAD, 0, &VarJackfruit,  0, validator_warn    },
  { "Kumquat",    DT_QUAD, 0, &VarKumquat,    0, validator_fail    },
  { "Lemon",      DT_QUAD, 0, &VarLemon,      0, NULL              }, /* test_inherit */
  { "Mango",      DT_QUAD, 0, &VarMango,      0, NULL              }, /* test_toggle */
  { "Nectarine",  DT_BOOL, 0, &VarNectarine,  0, NULL              },
  { NULL },
};
// clang-format on

static bool test_initial_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  printf("Apple = %d\n", VarApple);
  printf("Banana = %d\n", VarBanana);

  if ((VarApple != 0) || (VarBanana != 3))
  {
    printf("Error: initial values were wrong\n");
    return false;
  }

  cs_str_string_set(cs, "Apple", "ask-yes", err);
  cs_str_string_set(cs, "Banana", "ask-no", err);

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

  if (mutt_str_strcmp(value.data, "no") != 0)
  {
    printf("Apple's initial value is wrong: '%s'\n", value.data);
    FREE(&value.data);
    return false;
  }
  printf("Apple = %d\n", VarApple);
  printf("Apple's initial value is '%s'\n", value.data);

  mutt_buffer_reset(&value);
  rc = cs_str_initial_get(cs, "Banana", &value);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  if (mutt_str_strcmp(value.data, "ask-yes") != 0)
  {
    printf("Banana's initial value is wrong: '%s'\n", value.data);
    FREE(&value.data);
    return false;
  }
  printf("Banana = %d\n", VarBanana);
  printf("Banana's initial value is '%s'\n", NONULL(value.data));

  mutt_buffer_reset(&value);
  rc = cs_str_initial_set(cs, "Cherry", "ask-yes", &value);
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

  printf("Cherry = '%s'\n", VarCherry ? "yes" : "no");
  printf("Cherry's initial value is '%s'\n", NONULL(value.data));

  FREE(&value.data);
  return true;
}

static bool test_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "no", "yes", "ask-no", "ask-yes" };
  const char *invalid[] = {
    "nope", "ye", "", NULL,
  };
  char *name = "Damson";

  int rc;
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    VarDamson = ((i + 1) % 4);

    mutt_buffer_reset(err);
    rc = cs_str_string_set(cs, name, valid[i], err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      printf("%s\n", err->data);
      return false;
    }

    if (VarDamson == ((i + 1) % 4))
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = %d, set by '%s'\n", name, VarDamson, valid[i]);
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
      printf("%s = %d, set by '%s'\n", name, VarDamson, invalid[i]);
      printf("This test should have failed\n");
      return false;
    }
  }

  return true;
}

static bool test_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Elderberry";

  int valid[] = { MUTT_NO, MUTT_YES, MUTT_ASKNO, MUTT_ASKYES };

  int rc;
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    VarElderberry = i;
    mutt_buffer_reset(err);
    rc = cs_str_string_get(cs, name, err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      printf("Get failed: %s\n", err->data);
      return false;
    }
    printf("%s = %d, %s\n", name, VarElderberry, err->data);
  }

  VarElderberry = 4;
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
  char *name = "Fig";
  char value = MUTT_YES;

  VarFig = MUTT_NO;
  mutt_buffer_reset(err);
  int rc = cs_str_native_set(cs, name, value, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    return false;
  }

  if (VarFig != value)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("%s = %d, set to '%d'\n", name, VarFig, value);

  int invalid[] = { -1, 4 };
  for (unsigned int i = 0; i < mutt_array_size(invalid); i++)
  {
    VarFig = MUTT_NO;
    mutt_buffer_reset(err);
    rc = cs_str_native_set(cs, name, invalid[i], err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      printf("Expected error: %s\n", err->data);
    }
    else
    {
      printf("%s = %d, set by '%d'\n", name, VarFig, invalid[i]);
      printf("This test should have failed\n");
      return false;
    }
  }

  return true;
}

static bool test_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Guava";

  VarGuava = MUTT_YES;
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

  char *name = "Hawthorn";
  VarHawthorn = MUTT_YES;
  mutt_buffer_reset(err);

  int rc = cs_str_reset(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    return false;
  }

  if (VarHawthorn == MUTT_YES)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("Reset: %s = %d\n", name, VarHawthorn);

  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Ilama";
  VarIlama = MUTT_NO;
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, name, "yes", err);
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

  VarIlama = MUTT_NO;
  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, 1, err);
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
  VarJackfruit = MUTT_NO;
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "yes", err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %d\n", name, VarJackfruit);

  VarJackfruit = MUTT_NO;
  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, 1, err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("Native: %s = %d\n", name, VarJackfruit);

  name = "Kumquat";
  VarKumquat = MUTT_NO;
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "yes", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %d\n", name, VarKumquat);

  VarKumquat = MUTT_NO;
  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, 1, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("Native: %s = %d\n", name, VarKumquat);

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
  const char *parent = "Lemon";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarStr[] = {
    parent, NULL,
  };

  struct Account *ac = ac_create(cs, account, AccountVarStr);

  // set parent
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, parent, "yes", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, child, "no", err);
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

static bool test_toggle(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  struct ToggleTest
  {
    char before;
    char after;
  };

  struct ToggleTest tests[] = {
    { MUTT_NO, MUTT_YES }, { MUTT_YES, MUTT_NO }, { MUTT_ASKNO, MUTT_ASKYES }, { MUTT_ASKYES, MUTT_ASKNO },
  };

  char *name = "Mango";
  int rc;

  struct HashElem *he = cs_get_elem(cs, name);
  if (!he)
    return false;

  for (size_t i = 0; i < mutt_array_size(tests); i++)
  {
    char before = tests[i].before;
    char after = tests[i].after;
    printf("test %zd\n", i);

    VarMango = before;
    mutt_buffer_reset(err);
    intptr_t value = cs_he_native_get(cs, he, err);
    if (value == INT_MIN)
    {
      printf("Get failed: %s\n", err->data);
      return false;
    }

    char copy = value;
    if (copy != before)
    {
      printf("Initial value is wrong: %s\n", err->data);
      return false;
    }

    rc = quad_he_toggle(cs, he, err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      printf("Toggle failed: %s\n", err->data);
      return false;
    }

    if (VarMango != after)
    {
      printf("Toggle value is wrong: %s\n", err->data);
      return false;
    }
  }

  VarMango = 8;
  mutt_buffer_reset(err);
  rc = quad_he_toggle(cs, he, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }

  name = "Nectarine";
  he = cs_get_elem(cs, name);
  if (!he)
    return false;

  mutt_buffer_reset(err);
  rc = quad_he_toggle(cs, he, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }

  return true;
}

bool quad_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = mutt_mem_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_create(30);

  quad_init(cs);
  bool_init(cs);
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
  if (!test_toggle(cs, &err))
    return false;

  cs_free(&cs);
  FREE(&err.data);

  return true;
}
