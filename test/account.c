/**
 * @file
 * Test code for the Account object
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
#include "config/account.h"
#include "config/number.h"
#include "config/set.h"
#include "config/types.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "mutt_options.h"
#include "test/common.h"

static short VarApple;
static short VarBanana;
static short VarCherry;

// clang-format off
static struct ConfigDef Vars[] = {
  { "Apple",  DT_NUMBER, 0, &VarApple,  0, NULL },
  { "Banana", DT_NUMBER, 0, &VarBanana, 0, NULL },
  { "Cherry", DT_NUMBER, 0, &VarCherry, 0, NULL },
  { NULL },
};
// clang-format on

bool account_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_create(30);

  number_init(cs);
  if (!cs_register_variables(cs, Vars))
    return false;

  set_list(cs);

  cs_add_listener(cs, log_listener);

  const char *account = "damaged";
  const char *BrokenVarStr[] = {
    "Pineapple", NULL,
  };

  struct Account *ac = ac_create(cs, account, BrokenVarStr);
  if (!ac)
  {
    printf("Expected error:\n");
  }
  else
  {
    ac_free(cs, &ac);
    printf("This test should have failed\n");
    return false;
  }

  const char *AccountVarStr2[] = {
    "Apple", "Apple", NULL,
  };

  printf("Expect error for next test\n");
  ac = ac_create(cs, account, AccountVarStr2);
  if (ac)
  {
    ac_free(cs, &ac);
    printf("This test should have failed\n");
    return false;
  }

  account = "fruit";
  const char *AccountVarStr[] = {
    "Apple", "Cherry", NULL,
  };

  ac = ac_create(cs, account, AccountVarStr);
  if (!ac)
    return false;

  unsigned int index = 0;
  mutt_buffer_reset(&err);
  int rc = ac_set_value(ac, index, 42, &err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err.data);
  }

  mutt_buffer_reset(&err);
  rc = ac_set_value(ac, 99, 42, &err);
  if (CSR_RESULT(rc) == CSR_ERR_UNKNOWN)
  {
    printf("Expected error: %s\n", err.data);
  }
  else
  {
    printf("This test should have failed\n");
    return false;
  }

  mutt_buffer_reset(&err);
  rc = ac_get_value(ac, index, &err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err.data);
  }
  else
  {
    printf("%s = %s\n", AccountVarStr[index], err.data);
  }

  index++;
  mutt_buffer_reset(&err);
  rc = ac_get_value(ac, index, &err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("%s\n", err.data);
  }
  else
  {
    printf("%s = %s\n", AccountVarStr[index], err.data);
  }

  mutt_buffer_reset(&err);
  rc = ac_get_value(ac, 99, &err);
  if (CSR_RESULT(rc) == CSR_ERR_UNKNOWN)
  {
    printf("Expected error\n");
  }
  else
  {
    printf("This test should have failed\n");
    return false;
  }

  const char *name = "fruit:Apple";
  mutt_buffer_reset(&err);
  int result = cs_str_string_get(cs, name, &err);
  if (CSR_RESULT(result) == CSR_SUCCESS)
  {
    printf("%s = '%s'\n", name, err.data);
  }
  else
  {
    printf("%s\n", err.data);
    return false;
  }

  mutt_buffer_reset(&err);
  result = cs_str_native_set(cs, name, 42, &err);
  if (CSR_RESULT(result) == CSR_SUCCESS)
  {
    printf("Set %s\n", name);
  }
  else
  {
    printf("%s\n", err.data);
    return false;
  }

  struct HashElem *he = cs_get_elem(cs, name);
  if (!he)
    return false;

  mutt_buffer_reset(&err);
  result = cs_set_initial_value(cs, he, "42", &err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    printf("Expected error\n");
  }
  else
  {
    printf("This test should have failed\n");
    return false;
  }

  name = "Apple";
  he = cs_get_elem(cs, name);
  if (!he)
    return false;

  mutt_buffer_reset(&err);
  result = cs_he_native_set(cs, he, 42, &err);
  if (CSR_RESULT(result) == CSR_SUCCESS)
  {
    printf("Set %s\n", name);
  }
  else
  {
    printf("%s\n", err.data);
    return false;
  }

  ac_free(cs, &ac);
  cs_free(&cs);
  FREE(&err.data);

  return true;
}
