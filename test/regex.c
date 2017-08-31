/**
 * @file
 * Test code for the Regex object
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
#include "config/regex2.h"
#include "config/set.h"
#include "config/types.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "mutt_options.h"
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

// clang-format off
static struct VariableDef Vars[] = {
  { "Apple",      DT_REGEX, &VarApple,      IP "apple.*",     NULL              }, /* test_initial_values() */
  { "Banana",     DT_REGEX, &VarBanana,     IP "banana.*",    NULL              },
  { "Cherry",     DT_REGEX, &VarCherry,     0,                NULL              }, /* test_regex_set */
  { "Damson",     DT_REGEX, &VarDamson,     IP "damson.*",    NULL              },
  { "Elderberry", DT_REGEX, &VarElderberry, 0,                NULL              }, /* test_regex_get */
  { "Fig",        DT_REGEX, &VarFig,        IP "fig.*",       NULL              },
  { "Guava",      DT_REGEX, &VarGuava,      0,                NULL              },
  { "Hawthorn",   DT_REGEX, &VarHawthorn,   0,                NULL              }, /* test_native_set */
  { "Ilama",      DT_REGEX, &VarIlama,      IP "ilama.*",     NULL              },
  { "Jackfruit",  DT_REGEX, &VarJackfruit,  0,                NULL              }, /* test_native_get */
  { "Kumquat",    DT_REGEX, &VarKumquat,    IP "kumquat.*",   NULL              }, /* test_reset */
  { "Lemon",      DT_REGEX, &VarLemon,      IP "lemon.*",     validator_succeed }, /* test_validator */
  { "Mango",      DT_REGEX, &VarMango,      IP "mango.*",     validator_warn    },
  { "Nectarine",  DT_REGEX, &VarNectarine,  IP "nectarine.*", validator_fail    },
  { "Olive",      DT_REGEX, &VarOlive,      0,                NULL              }, /* test_inherit */
  { NULL },
};
// clang-format on

static bool test_initial_values_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  printf("Apple = %s\n", VarApple->pattern);
  printf("Banana = %s\n", VarBanana->pattern);

  return ((mutt_strcmp(VarApple->pattern, "apple.*") == 0) &&
          (mutt_strcmp(VarBanana->pattern, "banana.*") == 0));
}

static bool test_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "hello.*", "world.*", NULL };
  char *name = "Cherry";
  char *regex = NULL;

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

    regex = VarCherry ? VarCherry->pattern : NULL;
    if (mutt_strcmp(regex, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(regex), NONULL(valid[i]));
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

    regex = VarDamson ? VarDamson->pattern : NULL;
    if (mutt_strcmp(regex, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(regex), NONULL(valid[i]));
  }

  return true;
}

static bool test_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Elderberry";
  char *regex = NULL;

  mutt_buffer_reset(err);
  int rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  regex = VarElderberry ? VarElderberry->pattern : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(regex), err->data);

  name = "Fig";
  mutt_buffer_reset(err);
  rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  regex = VarFig ? VarFig->pattern : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(regex), err->data);

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
  regex = VarGuava ? VarGuava->pattern : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(regex), err->data);

  return true;
}

static bool test_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  struct Regex *r = regex_create("hello.*");
  char *name = "Hawthorn";
  char *regex = NULL;
  bool result = false;

  mutt_buffer_reset(err);
  int rc = cs_str_native_set(cs, name, (intptr_t) r, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    goto tns_out;
  }

  regex = VarHawthorn ? VarHawthorn->pattern : NULL;
  if (mutt_strcmp(regex, r->pattern) != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    goto tns_out;
  }
  printf("%s = '%s', set by '%s'\n", name, NONULL(regex), r->pattern);

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
  regex = VarIlama ? VarIlama->pattern : NULL;
  printf("%s = '%s', set by NULL\n", name, NONULL(regex));

  result = true;
tns_out:
  regex_free(&r);
  return result;
}

static bool test_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Jackfruit";

  int rc = cs_str_string_set(cs, name, "jackfruit.*", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;

  mutt_buffer_reset(err);
  intptr_t value = cs_str_native_get(cs, name, err);
  struct Regex *r = (struct Regex *) value;

  if (VarJackfruit != r)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  char *regex1 = VarJackfruit ? VarJackfruit->pattern : NULL;
  char *regex2 = r ? r->pattern : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(regex1), NONULL(regex2));

  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Kumquat";
  char *regex = NULL;

  mutt_buffer_reset(err);

  regex = VarKumquat ? VarKumquat->pattern : NULL;
  printf("Initial: %s = '%s'\n", name, NONULL(regex));
  int rc = cs_str_string_set(cs, name, "hello.*", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;
  regex = VarKumquat ? VarKumquat->pattern : NULL;
  printf("Set: %s = '%s'\n", name, NONULL(regex));

  rc = cs_reset_variable(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err->data);
    return false;
  }

  regex = VarKumquat ? VarKumquat->pattern : NULL;
  if (mutt_strcmp(regex, "kumquat.*") != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("Reset: %s = '%s'\n", name, NONULL(regex));

  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *regex = NULL;
  struct Regex *r = regex_create("world.*");
  bool result = false;

  char *name = "Lemon";
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, name, "hello.*", err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  regex = VarLemon ? VarLemon->pattern : NULL;
  printf("Regex: %s = %s\n", name, NONULL(regex));

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP r, err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  regex = VarLemon ? VarLemon->pattern : NULL;
  printf("Native: %s = %s\n", name, NONULL(regex));

  name = "Mango";
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "hello.*", err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  regex = VarMango ? VarMango->pattern : NULL;
  printf("Regex: %s = %s\n", name, NONULL(regex));

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP r, err);
  if (CSR_RESULT(rc) == CSR_SUCCESS)
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  regex = VarMango ? VarMango->pattern : NULL;
  printf("Native: %s = %s\n", name, NONULL(regex));

  name = "Nectarine";
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, name, "hello.*", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  regex = VarNectarine ? VarNectarine->pattern : NULL;
  printf("Regex: %s = %s\n", name, NONULL(regex));

  mutt_buffer_reset(err);
  rc = cs_str_native_set(cs, name, IP r, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  regex = VarNectarine ? VarNectarine->pattern : NULL;
  printf("Native: %s = %s\n", name, NONULL(regex));

  result = true;
tv_out:
  regex_free(&r);
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

  const char *AccountVarRegex[] = {
    parent, NULL,
  };

  struct Account *ac = ac_create(cs, account, AccountVarRegex);

  // set parent
  mutt_buffer_reset(err);
  int rc = cs_str_string_set(cs, parent, "hello.*", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Error: %s\n", err->data);
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(err);
  rc = cs_str_string_set(cs, child, "world.*", err);
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

bool regex_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_create(30);

  regex_init(cs);
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
