/**
 * @file
 * Test code for the Regex object
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
#include "config/lib.h"
#include "test/common.h"

static struct Regex *VarApple;
static struct Regex *VarBanana;
static struct Regex *VarCherry;
static struct Regex *VarDamson;
static struct Regex *VarElderberry;
static struct Regex *VarFig;
static struct Regex *VarGuava;
static struct Regex *VarHawthorn;
static struct Regex *VarIlama;
static struct Regex *VarJackfruit;
static struct Regex *VarKumquat;
static struct Regex *VarLemon;
static struct Regex *VarMango;
static struct Regex *VarNectarine;
static struct Regex *VarOlive;
static struct Regex *VarPapaya;
static struct Regex *VarQuince;
static struct Regex *VarRaspberry;
static struct Regex *VarStrawberry;

// clang-format off
static struct ConfigDef Vars[] = {
  { "Apple",      DT_REGEX, 0,                  &VarApple,      IP "apple.*",      NULL              }, /* test_initial_values */
  { "Banana",     DT_REGEX, 0,                  &VarBanana,     IP "banana.*",     NULL              },
  { "Cherry",     DT_REGEX, 0,                  &VarCherry,     IP "cherry.*",     NULL              },
  { "Damson",     DT_REGEX, 0,                  &VarDamson,     0,                 NULL              }, /* test_regex_set */
  { "Elderberry", DT_REGEX, DT_REGEX_NOSUB,     &VarElderberry, IP "elderberry.*", NULL              },
  { "Fig",        DT_REGEX, 0,                  &VarFig,        0,                 NULL              }, /* test_regex_get */
  { "Guava",      DT_REGEX, 0,                  &VarGuava,      IP "guava.*",      NULL              },
  { "Hawthorn",   DT_REGEX, 0,                  &VarHawthorn,   0,                 NULL              },
  { "Ilama",      DT_REGEX, DT_REGEX_ALLOW_NOT, &VarIlama,      0,                 NULL              }, /* test_native_set */
  { "Jackfruit",  DT_REGEX, 0,                  &VarJackfruit,  IP "jackfruit.*",  NULL              },
  { "Kumquat",    DT_REGEX, 0,                  &VarKumquat,    IP "kumquat.*",    NULL              },
  { "Lemon",      DT_REGEX, 0,                  &VarLemon,      0,                 NULL              }, /* test_native_get */
  { "Mango",      DT_REGEX, 0,                  &VarMango,      IP "mango.*",      NULL              }, /* test_reset */
  { "Nectarine",  DT_REGEX, 0,                  &VarNectarine,  IP "\\1",          NULL              },
  { "Olive",      DT_REGEX, 0,                  &VarOlive,      IP "olive.*",      validator_fail    },
  { "Papaya",     DT_REGEX, 0,                  &VarPapaya,     IP "papaya.*",     validator_succeed }, /* test_validator */
  { "Quince",     DT_REGEX, 0,                  &VarQuince,     IP "quince.*",     validator_warn    },
  { "Raspberry",  DT_REGEX, 0,                  &VarRaspberry,  IP "raspberry.*",  validator_fail    },
  { "Strawberry", DT_REGEX, 0,                  &VarStrawberry, 0,                 NULL              }, /* test_inherit */
  { NULL },
};
// clang-format on

static bool test_initial_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  TEST_MSG("Apple = %s\n", VarApple->pattern);
  TEST_MSG("Banana = %s\n", VarBanana->pattern);

  if (!TEST_CHECK(mutt_str_strcmp(VarApple->pattern, "apple.*") == 0))
  {
    TEST_MSG("Error: initial values were wrong\n");
    return false;
  }

  if (!TEST_CHECK(mutt_str_strcmp(VarBanana->pattern, "banana.*") == 0))
  {
    TEST_MSG("Error: initial values were wrong\n");
    return false;
  }

  cs_str_string_set(cs, "Apple", "car*", err);
  cs_str_string_set(cs, "Banana", "train*", err);

  struct Buffer value;
  mutt_buffer_init(&value);
  value.data = mutt_mem_calloc(1, 256);
  value.dsize = 256;
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

  if (!TEST_CHECK(mutt_str_strcmp(value.data, "apple.*") == 0))
  {
    TEST_MSG("Apple's initial value is wrong: '%s'\n", value.data);
    FREE(&value.data);
    return false;
  }
  TEST_MSG("Apple = '%s'\n", VarApple ? VarApple->pattern : "");
  TEST_MSG("Apple's initial value is %s\n", value.data);

  mutt_buffer_reset(&value);
  rc = cs_str_initial_get(cs, "Banana", &value);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  if (!TEST_CHECK(mutt_str_strcmp(value.data, "banana.*") == 0))
  {
    TEST_MSG("Banana's initial value is wrong: %s\n", value.data);
    FREE(&value.data);
    return false;
  }
  TEST_MSG("Banana = '%s'\n", VarBanana ? VarBanana->pattern : "");
  TEST_MSG("Banana's initial value is %s\n", NONULL(value.data));

  mutt_buffer_reset(&value);
  rc = cs_str_initial_set(cs, "Cherry", "up.*", &value);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", value.data);
    FREE(&value.data);
    return false;
  }

  mutt_buffer_reset(&value);
  rc = cs_str_initial_set(cs, "Cherry", "down.*", &value);
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

  TEST_MSG("Cherry = '%s'\n", VarCherry->pattern);
  TEST_MSG("Cherry's initial value is '%s'\n", NONULL(value.data));

  FREE(&value.data);
  log_line(__func__);
  return true;
}

static bool test_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "hello.*", "world.*", "world.*", "", NULL };
  char *name = "Damson";
  char *regex = NULL;

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

    regex = VarDamson ? VarDamson->pattern : NULL;
    if (!TEST_CHECK(mutt_str_strcmp(regex, valid[i]) == 0))
    {
      TEST_MSG("Value of %s wasn't changed\n", name);
      return false;
    }
    TEST_MSG("%s = '%s', set by '%s'\n", name, NONULL(regex), NONULL(valid[i]));
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

    if (rc & CSR_SUC_NO_CHANGE)
    {
      TEST_MSG("Value of %s wasn't changed\n", name);
      continue;
    }

    regex = VarElderberry ? VarElderberry->pattern : NULL;
    if (!TEST_CHECK(mutt_str_strcmp(regex, valid[i]) == 0))
    {
      TEST_MSG("Value of %s wasn't changed\n", name);
      return false;
    }
    TEST_MSG("%s = '%s', set by '%s'\n", name, NONULL(regex), NONULL(valid[i]));
  }

  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "\\1", err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    return false;
  }

  log_line(__func__);
  return true;
}

static bool test_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Fig";
  char *regex = NULL;

  mutt_buffer_reset(err);
  int rc = cs_str_string_get(cs, name, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Get failed: %s\n", err->data);
    return false;
  }
  regex = VarFig ? VarFig->pattern : NULL;
  TEST_MSG("%s = '%s', '%s'\n", name, NONULL(regex), err->data);

  name = "Guava";
  mutt_buffer_reset(err);
  rc = cs_str_string_get(cs, name, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Get failed: %s\n", err->data);
    return false;
  }
  regex = VarGuava ? VarGuava->pattern : NULL;
  TEST_MSG("%s = '%s', '%s'\n", name, NONULL(regex), err->data);

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
  regex = VarHawthorn ? VarHawthorn->pattern : NULL;
  TEST_MSG("%s = '%s', '%s'\n", name, NONULL(regex), err->data);

  log_line(__func__);
  return true;
}

static bool test_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  struct Regex *r = regex_new("hello.*", 0, err);
  char *name = "Ilama";
  char *regex = NULL;
  bool result = false;

  mutt_buffer_reset(err);
  int rc = cs_str_native_set(cs, name, (intptr_t) r, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
    goto tns_out;
  }

  regex = VarIlama ? VarIlama->pattern : NULL;
  if (!TEST_CHECK(mutt_str_strcmp(regex, r->pattern) == 0))
  {
    TEST_MSG("Value of %s wasn't changed\n", name);
    goto tns_out;
  }
  TEST_MSG("%s = '%s', set by '%s'\n", name, NONULL(regex), r->pattern);

  regex_free(&r);
  r = regex_new("!world.*", DT_REGEX_ALLOW_NOT, err);
  name = "Ilama";

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, (intptr_t) r, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
    goto tns_out;
  }
  TEST_MSG("'%s', not flag set to %d\n", VarIlama->pattern, VarIlama->not);

  name = "Jackfruit";
  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, 0, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
    goto tns_out;
  }

  if (!TEST_CHECK(VarJackfruit == NULL))
  {
    TEST_MSG("Value of %s wasn't changed\n", name);
    goto tns_out;
  }
  regex = VarJackfruit ? VarJackfruit->pattern : NULL;
  TEST_MSG("%s = '%s', set by NULL\n", name, NONULL(regex));

  regex_free(&r);
  r = regex_new("world.*", 0, err);
  r->pattern[0] = '\\';
  r->pattern[1] = '1';
  name = "Kumquat";

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, (intptr_t) r, err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    goto tns_out;
  }

  log_line(__func__);
  result = true;
tns_out:
  regex_free(&r);
  return result;
}

static bool test_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Lemon";

  int rc = cs_str_string_set(cs, name, "lemon.*", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    return false;

  mutt_buffer_reset(err);
  intptr_t value = cs_str_native_get(cs, name, err);
  struct Regex *r = (struct Regex *) value;

  if (!TEST_CHECK(VarLemon == r))
  {
    TEST_MSG("Get failed: %s\n", err->data);
    return false;
  }
  char *regex1 = VarLemon ? VarLemon->pattern : NULL;
  char *regex2 = r ? r->pattern : NULL;
  TEST_MSG("%s = '%s', '%s'\n", name, NONULL(regex1), NONULL(regex2));

  log_line(__func__);
  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Mango";

  mutt_buffer_reset(err);

  char *regex = VarMango ? VarMango->pattern : NULL;
  TEST_MSG("Initial: %s = '%s'\n", name, NONULL(regex));
  int rc = cs_str_string_set(cs, name, "hello.*", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    return false;
  regex = VarMango ? VarMango->pattern : NULL;
  TEST_MSG("Set: %s = '%s'\n", name, NONULL(regex));

  rc = cs_str_reset(cs, name, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
    return false;
  }

  regex = VarMango ? VarMango->pattern : NULL;
  if (!TEST_CHECK(mutt_str_strcmp(regex, "mango.*") == 0))
  {
    TEST_MSG("Value of %s wasn't changed\n", name);
    return false;
  }

  TEST_MSG("Reset: %s = '%s'\n", name, NONULL(regex));

  rc = cs_str_reset(cs, "Nectarine", err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    return false;
  }

  name = "Olive";
  mutt_buffer_reset(err);

  TEST_MSG("Initial: %s = '%s'\n", name, VarOlive->pattern);
  dont_fail = true;
  rc = cs_str_string_set(cs, name, "hel*o", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    return false;
  TEST_MSG("Set: %s = '%s'\n", name, VarOlive->pattern);
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

  if (!TEST_CHECK(mutt_str_strcmp(VarOlive->pattern, "hel*o") == 0))
  {
    TEST_MSG("Value of %s changed\n", name);
    return false;
  }

  TEST_MSG("Reset: %s = '%s'\n", name, VarOlive->pattern);

  log_line(__func__);
  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *regex = NULL;
  struct Regex *r = regex_new("world.*", 0, err);
  bool result = false;

  char *name = "Papaya";
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, name, "hello.*", err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    goto tv_out;
  }
  regex = VarPapaya ? VarPapaya->pattern : NULL;
  TEST_MSG("Regex: %s = %s\n", name, NONULL(regex));

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP r, err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    goto tv_out;
  }
  regex = VarPapaya ? VarPapaya->pattern : NULL;
  TEST_MSG("Native: %s = %s\n", name, NONULL(regex));

  name = "Quince";
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "hello.*", err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    goto tv_out;
  }
  regex = VarQuince ? VarQuince->pattern : NULL;
  TEST_MSG("Regex: %s = %s\n", name, NONULL(regex));

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP r, err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    goto tv_out;
  }
  regex = VarQuince ? VarQuince->pattern : NULL;
  TEST_MSG("Native: %s = %s\n", name, NONULL(regex));

  name = "Raspberry";
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "hello.*", err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    goto tv_out;
  }
  regex = VarRaspberry ? VarRaspberry->pattern : NULL;
  TEST_MSG("Regex: %s = %s\n", name, NONULL(regex));

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP r, err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s\n", err->data);
  }
  else
  {
    TEST_MSG("%s\n", err->data);
    goto tv_out;
  }
  regex = VarRaspberry ? VarRaspberry->pattern : NULL;
  TEST_MSG("Native: %s = %s\n", name, NONULL(regex));

  result = true;
tv_out:
  regex_free(&r);
  log_line(__func__);
  return result;
}

static void dump_native(struct ConfigSet *cs, const char *parent, const char *child)
{
  intptr_t pval = cs_str_native_get(cs, parent, NULL);
  intptr_t cval = cs_str_native_get(cs, child, NULL);

  struct Regex *pa = (struct Regex *) pval;
  struct Regex *ca = (struct Regex *) cval;

  char *pstr = pa ? pa->pattern : NULL;
  char *cstr = ca ? ca->pattern : NULL;

  TEST_MSG("%15s = %s\n", parent, NONULL(pstr));
  TEST_MSG("%15s = %s\n", child, NONULL(cstr));
}

static bool test_inherit(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  bool result = false;

  const char *account = "fruit";
  const char *parent = "Strawberry";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarRegex[] = {
    parent,
    NULL,
  };

  struct Account *ac = ac_new(cs, account, AccountVarRegex);

  // set parent
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, parent, "hello.*", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, child, "world.*", err);
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

void config_regex(void)
{
  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = mutt_mem_calloc(1, 256);
  err.dsize = 256;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new(30);

  regex_init(cs);
  dont_fail = true;
  if (!cs_register_variables(cs, Vars, 0))
    return;
  dont_fail = false;

  cs_add_observer(cs, log_observer);

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
