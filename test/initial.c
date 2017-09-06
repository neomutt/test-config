/**
 * @file
 * Test code for pre-setting initial values
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

// clang-format off
static struct ConfigDef Vars[] = {
  { "Apple",  DT_STRING, 0, &VarApple,  IP "apple", NULL },
  { "Banana", DT_STRING, 0, &VarBanana, 0,          NULL },
  { "Cherry", DT_STRING, 0, &VarCherry, 0,          NULL },
  { NULL },
};
// clang-format on

static bool test_set_initial(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  struct HashElem *he_a = cs_get_elem(cs, "Apple");
  if (!he_a)
    return false;

  const char *aval = "pie";
  int rc = cs_set_initial_value(cs, he_a, aval, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    printf("Expected error: %s\n", err->data);

  struct HashElem *he_b = cs_get_elem(cs, "Banana");
  if (!he_b)
    return false;

  const char *bval = "split";
  rc = cs_set_initial_value(cs, he_b, bval, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;

  struct HashElem *he_c = cs_get_elem(cs, "Cherry");
  if (!he_c)
    return false;

  const char *cval = "blossom";
  rc = cs_set_initial_value(cs, he_c, cval, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;

  rc = cs_set_initial_value(cs, he_c, cval, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    printf("Expected error: %s\n", err->data);

  printf("Apple = %s\n", VarApple);
  printf("Banana = %s\n", VarBanana);

  return ((mutt_strcmp(VarApple, aval) != 0) && (mutt_strcmp(VarBanana, bval) == 0));
}

bool initial_test(void)
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

  if (!test_set_initial(cs, &err))
  {
    cs_free(&cs);
    FREE(&err.data);
    return false;
  }

  cs_free(&cs);
  FREE(&err.data);

  return true;
}
