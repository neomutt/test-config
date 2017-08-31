/**
 * @file
 * Test code for the Mbtable object
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
#include "config/mbtable.h"
#include "config/account.h"
#include "config/set.h"
#include "config/types.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "mutt_options.h"
#include "test/common.h"

static struct MbTable *VarApple;
static struct MbTable *VarBanana;
static struct MbTable *VarCherry;
static struct MbTable *VarDamson;
static struct MbTable *VarElderberry;
static struct MbTable *VarFig;
static struct MbTable *VarGuava;
static struct MbTable *VarHawthorn;
static struct MbTable *VarIlama;
static struct MbTable *VarJackfruit;
static struct MbTable *VarKumquat;
static struct MbTable *VarLemon;
static struct MbTable *VarMango;
static struct MbTable *VarNectarine;
static struct MbTable *VarOlive;

// clang-format off
static struct VariableDef Vars[] = {
  { "Apple",      DT_MBTABLE, &VarApple,      IP "apple",     NULL              }, /* test_initial_values() */
  { "Banana",     DT_MBTABLE, &VarBanana,     IP "banana",    NULL              },
  { "Cherry",     DT_MBTABLE, &VarCherry,     0,              NULL              }, /* test_mbtable_set */
  { "Damson",     DT_MBTABLE, &VarDamson,     IP "damson",    NULL              },
  { "Elderberry", DT_MBTABLE, &VarElderberry, 0,              NULL              }, /* test_mbtable_get */
  { "Fig",        DT_MBTABLE, &VarFig,        IP "fig",       NULL              },
  { "Guava",      DT_MBTABLE, &VarGuava,      0,              NULL              },
  { "Hawthorn",   DT_MBTABLE, &VarHawthorn,   0,              NULL              }, /* test_native_set */
  { "Ilama",      DT_MBTABLE, &VarIlama,      IP "ilama",     NULL              },
  { "Jackfruit",  DT_MBTABLE, &VarJackfruit,  0,              NULL              }, /* test_native_get */
  { "Kumquat",    DT_MBTABLE, &VarKumquat,    IP "kumquat",   NULL              }, /* test_reset */
  { "Lemon",      DT_MBTABLE, &VarLemon,      IP "lemon",     validator_succeed }, /* test_validator */
  { "Mango",      DT_MBTABLE, &VarMango,      IP "mango",     validator_warn    },
  { "Nectarine",  DT_MBTABLE, &VarNectarine,  IP "nectarine", validator_fail    },
  { "Olive",      DT_MBTABLE, &VarOlive,      0,              NULL              }, /* test_inherit */
  { NULL },
};
// clang-format on

static bool test_initial_values_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  printf("Apple = %s\n", VarApple->orig_str);
  printf("Banana = %s\n", VarBanana->orig_str);

  return ((mutt_strcmp(VarApple->orig_str, "apple") == 0) &&
          (mutt_strcmp(VarBanana->orig_str, "banana") == 0));
}

static bool test_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "hello", "world", "", NULL };
  char *name = "Cherry";
  char *mb = NULL;

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

    mb = VarCherry ? VarCherry->orig_str : NULL;
    if (mutt_strcmp(mb, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(mb), NONULL(valid[i]));
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

    const char *orig_str = VarDamson ? VarDamson->orig_str : NULL;
    if (mutt_strcmp(orig_str, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(orig_str), NONULL(valid[i]));
  }

  return true;
}

static bool test_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Elderberry";
  char *mb = NULL;

  mutt_buffer_reset(err);
  int rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  mb = VarElderberry ? VarElderberry->orig_str : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(mb), err->data);

  name = "Fig";
  mutt_buffer_reset(err);
  rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  mb = VarFig ? VarFig->orig_str : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(mb), err->data);

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
  mb = VarGuava ? VarGuava->orig_str : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(mb), err->data);

  return true;
}

static bool test_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  struct MbTable *t = mbtable_create("hello");
  char *name = "Hawthorn";
  char *mb = NULL;
  bool result = false;

  mutt_buffer_reset(err);
  int rc = cs_str_native_set(cs, name, (intptr_t) t, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    goto tns_out;
  }

  mb = VarHawthorn ? VarHawthorn->orig_str : NULL;
  if (mutt_strcmp(mb, t->orig_str) != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    goto tns_out;
  }
  printf("%s = '%s', set by '%s'\n", name, NONULL(mb), t->orig_str);

  name = "Ilama";
  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, 0, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    goto tns_out;
  }

  if (VarIlama != NULL)
  {
    printf("Value of %s wasn't changed\n", name);
    goto tns_out;
  }
  mb = VarIlama ? VarIlama->orig_str : NULL;
  printf("%s = '%s', set by NULL\n", name, NONULL(mb));

  result = true;
tns_out:
  mbtable_free(&t);
  return result;
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
  struct MbTable *t = (struct MbTable *) value;

  if (VarJackfruit != t)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  char *mb1 = VarJackfruit ? VarJackfruit->orig_str : NULL;
  char *mb2 = t ? t->orig_str : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(mb1), NONULL(mb2));

  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Kumquat";
  char *mb = NULL;

  mutt_buffer_reset(err);

  mb = VarKumquat ? VarKumquat->orig_str : NULL;
  printf("Initial: %s = '%s'\n", name, NONULL(mb));
  int rc = cs_str_string_set(cs, name, "hello", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;
  mb = VarKumquat ? VarKumquat->orig_str : NULL;
  printf("Set: %s = '%s'\n", name, NONULL(mb));

  rc = cs_reset_variable(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    return false;
  }

  mb = VarKumquat ? VarKumquat->orig_str : NULL;
  if (mutt_strcmp(mb, "kumquat") != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("Reset: %s = '%s'\n", name, NONULL(mb));

  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  struct MbTable *t = mbtable_create("world");
  bool result = false;

  char *name = "Lemon";
  char *mb = NULL;
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, name, "hello", err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  mb = VarLemon ? VarLemon->orig_str : NULL;
  printf("MbTable: %s = %s\n", name, NONULL(mb));

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP t, err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  mb = VarLemon ? VarLemon->orig_str : NULL;
  printf("Native: %s = %s\n", name, NONULL(mb));

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
    goto tv_out;
  }
  mb = VarMango ? VarMango->orig_str : NULL;
  printf("MbTable: %s = %s\n", name, NONULL(mb));

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP t, err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  mb = VarMango ? VarMango->orig_str : NULL;
  printf("Native: %s = %s\n", name, NONULL(mb));

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
    goto tv_out;
  }
  mb = VarNectarine ? VarNectarine->orig_str : NULL;
  printf("MbTable: %s = %s\n", name, NONULL(mb));

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP t, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  mb = VarNectarine ? VarNectarine->orig_str : NULL;
  printf("Native: %s = %s\n", name, NONULL(mb));

  result = true;
tv_out:
  mbtable_free(&t);
  return result;
}

static void dump_native(struct ConfigSet *cs, const char *parent, const char *child)
{
  intptr_t pval = cs_str_native_get(cs, parent, NULL);
  intptr_t cval = cs_str_native_get(cs, child, NULL);

  struct MbTable *pa = (struct MbTable *) pval;
  struct MbTable *ca = (struct MbTable *) cval;

  char *pstr = pa ? pa->orig_str : NULL;
  char *cstr = ca ? ca->orig_str : NULL;

  printf("%15s = %s\n", parent, NONULL(pstr));
  printf("%15s = %s\n", child, NONULL(cstr));
}

static bool test_inherit(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  bool result = false;

  const char *account = "fruit";
  const char *parent = "Olive";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarMb[] = {
    parent, NULL,
  };

  struct Account *ac = ac_create(cs, account, AccountVarMb);

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

bool mbtable_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_create(30);

  mbtable_init(cs);
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
