/**
 * @file
 * Test code for the Address object
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
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "mutt/mutt.h"
#include "config/lib.h"
#include "email/address.h"
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
static struct Address *VarQuince;

// clang-format off
static struct ConfigDef Vars[] = {
  { "Apple",      DT_ADDRESS, 0, &VarApple,      IP "apple@example.com",      NULL              }, /* test_initial_values */
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
  { "Mango",      DT_ADDRESS, 0, &VarMango,      IP "mango@example.com",      validator_fail    },
  { "Nectarine",  DT_ADDRESS, 0, &VarNectarine,  IP "nectarine@example.com",  validator_succeed }, /* test_validator */
  { "Olive",      DT_ADDRESS, 0, &VarOlive,      IP "olive@example.com",      validator_warn    },
  { "Papaya",     DT_ADDRESS, 0, &VarPapaya,     IP "papaya@example.com",     validator_fail    },
  { "Quince",     DT_ADDRESS, 0, &VarQuince,     0,                           NULL              }, /* test_inherit */
  { NULL },
};
// clang-format on

static bool test_initial_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  TEST_MSG("Apple = '%s'\n", VarApple->mailbox);
  TEST_MSG("Banana = '%s'\n", VarBanana->mailbox);

  const char *apple_orig = "apple@example.com";
  const char *banana_orig = "banana@example.com";

  if (!TEST_CHECK(mutt_str_strcmp(VarApple->mailbox, apple_orig) == 0))
  {
    TEST_MSG("Error: initial values were wrong\n");
    return false;
  }

  if (!TEST_CHECK(mutt_str_strcmp(VarBanana->mailbox, banana_orig) == 0))
  {
    TEST_MSG("Error: initial values were wrong\n");
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
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  if (!TEST_CHECK(mutt_str_strcmp(value.data, apple_orig) == 0))
  {
    TEST_MSG("Apple's initial value is wrong: '%s'\n", value.data);
    FREE(&value.data);
    return false;
  }
  TEST_MSG("Apple = '%s'\n", VarApple->mailbox);
  TEST_MSG("Apple's initial value is '%s'\n", value.data);

  mutt_buffer_reset(&value);
  rc = cs_str_initial_get(cs, "Banana", &value);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  if (!TEST_CHECK(mutt_str_strcmp(value.data, banana_orig) == 0))
  {
    TEST_MSG("Banana's initial value is wrong: '%s'\n", value.data);
    FREE(&value.data);
    return false;
  }
  TEST_MSG("Banana = '%s'\n", VarBanana ? VarBanana->mailbox : "");
  TEST_MSG("Banana's initial value is '%s'\n", NONULL(value.data));

  mutt_buffer_reset(&value);
  rc = cs_str_initial_set(cs, "Cherry", "john@doe.com", &value);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  mutt_buffer_reset(&value);
  rc = cs_str_initial_set(cs, "Cherry", "jane@doe.com", &value);
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

  TEST_MSG("Cherry = '%s'\n", VarCherry ? VarCherry->mailbox : "");
  TEST_MSG("Cherry's initial value is '%s'\n", NONULL(value.data));

  FREE(&value.data);
  log_line(__func__);
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
    if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    {
      TEST_MSG("%s\n", err->data);
      return false;
    }

    addr = VarDamson ? VarDamson->mailbox : NULL;
    if (!TEST_CHECK(mutt_str_strcmp(addr, valid[i]) == 0))
    {
      TEST_MSG("Value of %s wasn't changed\n", name);
      return false;
    }
    TEST_MSG("%s = '%s', set by '%s'\n", name, NONULL(addr), NONULL(valid[i]));
  }

  name = "Elderberry";
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    rc = cs_str_string_set(cs, name, valid[i], err);
    if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    {
      TEST_MSG("%s\n", err->data);
      return false;
    }

    addr = VarElderberry ? VarElderberry->mailbox : NULL;
    if (!TEST_CHECK(mutt_str_strcmp(addr, valid[i]) == 0))
    {
      TEST_MSG("Value of %s wasn't changed\n", name);
      return false;
    }
    TEST_MSG("%s = '%s', set by '%s'\n", name, NONULL(addr), NONULL(valid[i]));
  }

  log_line(__func__);
  return true;
}

static bool test_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Fig";
  char *addr = NULL;

  mutt_buffer_reset(err);
  int rc = cs_str_string_get(cs, name, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Get failed: %s\n", err->data);
    return false;
  }
  addr = VarFig ? VarFig->mailbox : NULL;
  TEST_MSG("%s = '%s', '%s'\n", name, NONULL(addr), err->data);

  name = "Guava";
  mutt_buffer_reset(err);
  rc = cs_str_string_get(cs, name, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Get failed: %s\n", err->data);
    return false;
  }
  addr = VarGuava ? VarGuava->mailbox : NULL;
  TEST_MSG("%s = '%s', '%s'\n", name, NONULL(addr), err->data);

  name = "Hawthorn";
  rc = cs_str_string_set(cs, name, "hawthorn", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    return false;

  mutt_buffer_reset(err);
  rc = cs_str_string_get(cs, name, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Get failed: %s\n", err->data);
    return false;
  }
  addr = VarHawthorn ? VarHawthorn->mailbox : NULL;
  TEST_MSG("%s = '%s', '%s'\n", name, NONULL(addr), err->data);

  log_line(__func__);
  return true;
}

static bool test_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  struct Address *a = address_new("hello@example.com");
  char *name = "Ilama";
  char *addr = NULL;
  bool result = false;

  mutt_buffer_reset(err);
  int rc = cs_str_native_set(cs, name, (intptr_t) a, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
    goto tbns_out;
  }

  addr = VarIlama ? VarIlama->mailbox : NULL;
  if (!TEST_CHECK(mutt_str_strcmp(addr, a->mailbox) == 0))
  {
    TEST_MSG("Value of %s wasn't changed\n", name);
    goto tbns_out;
  }
  TEST_MSG("%s = '%s', set by '%s'\n", name, NONULL(addr), a->mailbox);

  name = "Jackfruit";
  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, 0, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
    goto tbns_out;
  }

  if (!TEST_CHECK(VarJackfruit == NULL))
  {
    TEST_MSG("Value of %s wasn't changed\n", name);
    goto tbns_out;
  }
  addr = VarJackfruit ? VarJackfruit->mailbox : NULL;
  TEST_MSG("%s = '%s', set by NULL\n", name, NONULL(addr));

  log_line(__func__);
  result = true;
tbns_out:
  address_free(&a);
  return result;
}

static bool test_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Kumquat";

  if (!TEST_CHECK(cs_str_string_set(cs, name, "kumquat@example.com", err) != INT_MIN))
    return false;

  mutt_buffer_reset(err);
  intptr_t value = cs_str_native_get(cs, name, err);
  struct Address *a = (struct Address *) value;

  if (!TEST_CHECK(VarKumquat == a))
  {
    TEST_MSG("Get failed: %s\n", err->data);
    return false;
  }
  char *addr1 = VarKumquat ? VarKumquat->mailbox : NULL;
  char *addr2 = a ? a->mailbox : NULL;
  TEST_MSG("%s = '%s', '%s'\n", name, NONULL(addr1), NONULL(addr2));

  log_line(__func__);
  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Lemon";

  mutt_buffer_reset(err);

  char *addr = VarLemon ? VarLemon->mailbox : NULL;
  TEST_MSG("Initial: %s = '%s'\n", name, NONULL(addr));
  int rc = cs_str_string_set(cs, name, "hello@example.com", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    return false;
  addr = VarLemon ? VarLemon->mailbox : NULL;
  TEST_MSG("Set: %s = '%s'\n", name, NONULL(addr));

  rc = cs_str_reset(cs, name, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
    return false;
  }

  addr = VarLemon ? VarLemon->mailbox : NULL;
  if (!TEST_CHECK(mutt_str_strcmp(addr, "lemon@example.com") == 0))
  {
    TEST_MSG("Value of %s wasn't changed\n", name);
    return false;
  }

  TEST_MSG("Reset: %s = '%s'\n", name, NONULL(addr));

  name = "Mango";
  mutt_buffer_reset(err);

  TEST_MSG("Initial: %s = '%s'\n", name, VarMango->mailbox);
  dont_fail = true;
  rc = cs_str_string_set(cs, name, "john@example.com", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    return false;
  TEST_MSG("Set: %s = '%s'\n", name, VarMango->mailbox);
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

  if (!TEST_CHECK(mutt_str_strcmp(VarMango->mailbox, "john@example.com") == 0))
  {
    TEST_MSG("Value of %s changed\n", name);
    return false;
  }

  TEST_MSG("Reset: %s = '%s'\n", name, VarMango->mailbox);

  log_line(__func__);
  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *addr = NULL;
  struct Address *a = address_new("world@example.com");
  bool result = false;

  char *name = "Nectarine";
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, name, "hello@example.com", err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    goto tv_out;
  }
  addr = VarNectarine ? VarNectarine->mailbox : NULL;
  TEST_MSG("Address: %s = %s\n", name, NONULL(addr));

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP a, err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    goto tv_out;
  }
  addr = VarNectarine ? VarNectarine->mailbox : NULL;
  TEST_MSG("Native: %s = %s\n", name, NONULL(addr));

  name = "Olive";
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "hello@example.com", err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    goto tv_out;
  }
  addr = VarOlive ? VarOlive->mailbox : NULL;
  TEST_MSG("Address: %s = %s\n", name, NONULL(addr));

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP a, err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    goto tv_out;
  }
  addr = VarOlive ? VarOlive->mailbox : NULL;
  TEST_MSG("Native: %s = %s\n", name, NONULL(addr));

  name = "Papaya";
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "hello@example.com", err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    goto tv_out;
  }
  addr = VarPapaya ? VarPapaya->mailbox : NULL;
  TEST_MSG("Address: %s = %s\n", name, NONULL(addr));

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP a, err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    goto tv_out;
  }
  addr = VarPapaya ? VarPapaya->mailbox : NULL;
  TEST_MSG("Native: %s = %s\n", name, NONULL(addr));

  result = true;
tv_out:
  address_free(&a);
  log_line(__func__);
  return result;
}

static void dump_native(struct ConfigSet *cs, const char *parent, const char *child)
{
  intptr_t pval = cs_str_native_get(cs, parent, NULL);
  intptr_t cval = cs_str_native_get(cs, child, NULL);

  struct Address *pa = (struct Address *) pval;
  struct Address *ca = (struct Address *) cval;

  char *pstr = pa ? pa->mailbox : NULL;
  char *cstr = ca ? ca->mailbox : NULL;

  TEST_MSG("%15s = %s\n", parent, NONULL(pstr));
  TEST_MSG("%15s = %s\n", child, NONULL(cstr));
}

static bool test_inherit(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  bool result = false;

  const char *account = "fruit";
  const char *parent = "Quince";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarAddr[] = {
    parent,
    NULL,
  };

  struct Account *ac = ac_new(cs, account, AccountVarAddr);

  // set parent
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, parent, "hello@example.com", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, child, "world@example.com", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // reset child
  mutt_buffer_reset(err);
  rc = cs_str_reset(cs, child, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // reset parent
  mutt_buffer_reset(err);
  rc = cs_str_reset(cs, parent, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  log_line(__func__);
  result = true;
ti_out:
  ac_free(cs, &ac);
  return result;
}

void config_address(void)
{
  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = mutt_mem_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new(30);

  address_init(cs);
  dont_fail = true;
  if (!cs_register_variables(cs, Vars, 0))
    return;
  dont_fail = false;

  cs_add_listener(cs, log_listener);

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
