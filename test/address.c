/**
 * @file
 * Test code for the Address object
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
#include "config/address.h"
#include "config/set.h"
#include "config/types.h"
#include "test/common.h"

static struct Address *VarApple;
static struct Address *VarBanana;
static struct Address *VarCherry;
static struct Address *VarDamson;
static struct Address *VarElderberry;
static struct Address *VarFig;
static struct Address *VarGuava;
static struct Address *VarHawthorn;
static struct Address *VarIlama;
static struct Address *VarJackfruit;
static struct Address *VarKumquat;
static struct Address *VarLemon;
static struct Address *VarMango;
static struct Address *VarNectarine;
static struct Address *VarOlive;
static struct Address *VarPapaya;

// clang-format off
static struct ConfigDef Vars[] = {
  { "Apple",      DT_ADDRESS, 0, &VarApple,      IP "apple@example.com",      NULL              }, /* test_initial_values() */
  { "Banana",     DT_ADDRESS, 0, &VarBanana,     IP "banana@example.com",     NULL              },
  { "Cherry",     DT_ADDRESS, 0, &VarCherry,     IP "cherry@example.com",     NULL              },
  { "Damson",     DT_ADDRESS, 0, &VarDamson,     0,                           NULL              }, /* test_address_set */
  { "Elderberry", DT_ADDRESS, 0, &VarElderberry, IP "elderberry@example.com", NULL              },
  { "Fig",        DT_ADDRESS, 0, &VarFig,        0,                           NULL              }, /* test_address_get */
  { "Guava",      DT_ADDRESS, 0, &VarGuava,      IP "guava@example.com",      NULL              },
  { "Hawthorn",   DT_ADDRESS, 0, &VarHawthorn,   0,                           NULL              },
  { "Ilama",      DT_ADDRESS, 0, &VarIlama,      0,                           NULL              }, /* test_native_set */
  { "Jackfruit",  DT_ADDRESS, 0, &VarJackfruit,  IP "jackfruit@example.com",  NULL              },
  { "Kumquat",    DT_ADDRESS, 0, &VarKumquat,    0,                           NULL              }, /* test_native_get */
  { "Lemon",      DT_ADDRESS, 0, &VarLemon,      IP "lemon@example.com",      NULL              }, /* test_reset */
  { "Mango",      DT_ADDRESS, 0, &VarMango,      IP "mango@example.com",      validator_succeed }, /* test_validator */
  { "Nectarine",  DT_ADDRESS, 0, &VarNectarine,  IP "nectarine@example.com",  validator_warn    },
  { "Olive",      DT_ADDRESS, 0, &VarOlive,      IP "olive@example.com",      validator_fail    },
  { "Papaya",     DT_ADDRESS, 0, &VarPapaya,     0,                           NULL              }, /* test_inherit */
  { NULL },
};
// clang-format on

static bool test_initial_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  printf("Apple = '%s'\n", VarApple->personal);
  printf("Banana = '%s'\n", VarBanana->personal);

  const char *apple_orig = "apple@example.com";
  const char *banana_orig = "banana@example.com";

  if ((mutt_str_strcmp(VarApple->personal, apple_orig) != 0) ||
      (mutt_str_strcmp(VarBanana->personal, banana_orig) != 0))
  {
    printf("Error: initial values were wrong\n");
    return false;
  }

  cs_str_string_set(cs, "Apple", "granny@smith.com", err);
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

  if (mutt_str_strcmp(value.data, apple_orig) != 0)
  {
    printf("Apple's initial value is wrong: '%s'\n", value.data);
    FREE(&value.data);
    return false;
  }
  printf("Apple = '%s'\n", VarApple->personal);
  printf("Apple's initial value is '%s'\n", value.data);

  mutt_buffer_reset(&value);
  rc = cs_str_initial_get(cs, "Banana", &value);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  if (mutt_str_strcmp(value.data, banana_orig) != 0)
  {
    printf("Banana's initial value is wrong: '%s'\n", value.data);
    FREE(&value.data);
    return false;
  }
  printf("Banana = '%s'\n", VarBanana ? VarBanana->personal : "");
  printf("Banana's initial value is '%s'\n", NONULL(value.data));

  mutt_buffer_reset(&value);
  rc = cs_str_initial_set(cs, "Cherry", "john@doe.com", &value);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  mutt_buffer_reset(&value);
  rc = cs_str_initial_set(cs, "Cherry", "jane@doe.com", &value);
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

  printf("Cherry = '%s'\n", VarCherry ? VarCherry->mailbox : "");
  printf("Cherry's initial value is '%s'\n", NONULL(value.data));

  FREE(&value.data);
  return true;
}

static bool test_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "hello@example.com", "world@example.com", NULL };
  char *name = "Damson";
  char *addr = NULL;

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

    addr = VarDamson ? VarDamson->personal : NULL;
    if (mutt_str_strcmp(addr, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(addr), NONULL(valid[i]));
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

    addr = VarElderberry ? VarElderberry->personal : NULL;
    if (mutt_str_strcmp(addr, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(addr), NONULL(valid[i]));
  }

  return true;
}

static bool test_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Fig";
  char *addr = NULL;

  mutt_buffer_reset(err);
  int rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  addr = VarFig ? VarFig->personal : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(addr), err->data);

  name = "Guava";
  mutt_buffer_reset(err);
  rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  addr = VarGuava ? VarGuava->personal : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(addr), err->data);

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
  addr = VarHawthorn ? VarHawthorn->personal : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(addr), err->data);

  return true;
}

static bool test_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  struct Address *a = address_create("hello@example.com");
  char *name = "Ilama";
  char *addr = NULL;
  bool result = false;

  mutt_buffer_reset(err);
  int rc = cs_str_native_set(cs, name, (intptr_t) a, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    goto tbns_out;
  }

  addr = VarIlama ? VarIlama->personal : NULL;
  if (mutt_str_strcmp(addr, a->personal) != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    goto tbns_out;
  }
  printf("%s = '%s', set by '%s'\n", name, NONULL(addr), a->personal);

  name = "Jackfruit";
  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, 0, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    goto tbns_out;
  }

  if (VarJackfruit != NULL)
  {
    printf("Value of %s wasn't changed\n", name);
    goto tbns_out;
  }
  addr = VarJackfruit ? VarJackfruit->personal : NULL;
  printf("%s = '%s', set by NULL\n", name, NONULL(addr));

  result = true;
tbns_out:
  address_free(&a);
  return result;
}

static bool test_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Kumquat";

  if (cs_str_string_set(cs, name, "kumquat@example.com", err) == INT_MIN)
    return false;

  mutt_buffer_reset(err);
  intptr_t value = cs_str_native_get(cs, name, err);
  struct Address *a = (struct Address *) value;

  if (VarKumquat != a)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  char *addr1 = VarKumquat ? VarKumquat->personal : NULL;
  char *addr2 = a ? a->personal : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(addr1), NONULL(addr2));

  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Lemon";
  char *addr = NULL;

  mutt_buffer_reset(err);

  addr = VarLemon ? VarLemon->personal : NULL;
  printf("Initial: %s = '%s'\n", name, NONULL(addr));
  int rc = cs_str_string_set(cs, name, "hello@example.com", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;
  addr = VarLemon ? VarLemon->personal : NULL;
  printf("Set: %s = '%s'\n", name, NONULL(addr));

  rc = cs_str_reset(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    return false;
  }

  addr = VarLemon ? VarLemon->personal : NULL;
  if (mutt_str_strcmp(addr, "lemon@example.com") != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("Reset: %s = '%s'\n", name, NONULL(addr));

  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *addr = NULL;
  struct Address *a = address_create("world@example.com");
  bool result = false;

  char *name = "Mango";
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, name, "hello@example.com", err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  addr = VarMango ? VarMango->personal : NULL;
  printf("Address: %s = %s\n", name, NONULL(addr));

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP a, err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  addr = VarMango ? VarMango->personal : NULL;
  printf("Native: %s = %s\n", name, NONULL(addr));

  name = "Nectarine";
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "hello@example.com", err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  addr = VarNectarine ? VarNectarine->personal : NULL;
  printf("Address: %s = %s\n", name, NONULL(addr));

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP a, err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  addr = VarNectarine ? VarNectarine->personal : NULL;
  printf("Native: %s = %s\n", name, NONULL(addr));

  name = "Olive";
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "hello@example.com", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  addr = VarOlive ? VarOlive->personal : NULL;
  printf("Address: %s = %s\n", name, NONULL(addr));

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP a, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  addr = VarOlive ? VarOlive->personal : NULL;
  printf("Native: %s = %s\n", name, NONULL(addr));

  result = true;
tv_out:
  address_free(&a);
  return result;
}

static void dump_native(struct ConfigSet *cs, const char *parent, const char *child)
{
  intptr_t pval = cs_str_native_get(cs, parent, NULL);
  intptr_t cval = cs_str_native_get(cs, child, NULL);

  struct Address *pa = (struct Address *) pval;
  struct Address *ca = (struct Address *) cval;

  char *pstr = pa ? pa->personal : NULL;
  char *cstr = ca ? ca->personal : NULL;

  printf("%15s = %s\n", parent, NONULL(pstr));
  printf("%15s = %s\n", child, NONULL(cstr));
}

static bool test_inherit(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  bool result = false;

  const char *account = "fruit";
  const char *parent = "Papaya";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarAddr[] = {
    parent, NULL,
  };

  struct Account *ac = ac_create(cs, account, AccountVarAddr);

  // set parent
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, parent, "hello@example.com", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, child, "world@example.com", err);
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

bool address_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = mutt_mem_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_create(30);

  address_init(cs);
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
