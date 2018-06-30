/**
 * @file
 * Test code for the Slist object
 *
 * @authors
 * Copyright (C) 2018 Richard Russon <rich@flatcap.org>
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
#include "mutt/mapping.h"
#include "mutt/memory.h"
#include "mutt/string2.h"
#include "config/account.h"
#include "config/bool.h"
#include "config/set.h"
#include "config/slist.h"
#include "config/types.h"
#include "test/common.h"

struct Slist *VarApple;
struct Slist *VarBanana;
struct Slist *VarCherry;

// clang-format off
static struct ConfigDef Vars[] = {
  { "Apple",  DT_SLIST, SLIST_SEP_COLON, &VarApple,  IP "apple:banana:cherry", NULL }, /* test_initial_values */
  { "Banana", DT_SLIST, SLIST_SEP_COMMA, &VarBanana, IP "apple,banana,cherry", NULL },
  { "Cherry", DT_SLIST, SLIST_SEP_SPACE, &VarCherry, IP "apple banana cherry", NULL },
  { NULL },
};
// clang-format on

static bool test_initial_values(struct ConfigSet *cs, struct Buffer *err)
{
  printf("Apple:  %ld\n", VarApple->count);
  printf("Banana: %ld\n", VarBanana->count);
  printf("Cherry: %ld\n", VarCherry->count);

  slist_add_string(VarApple, "damson");
  slist_remove_string(VarBanana, "banana");
  set_list(cs);
  printf("Apple:  %ld\n", VarApple->count);
  printf("Banana: %ld\n", VarBanana->count);
  printf("Cherry: %d\n", slist_is_member(VarCherry, "cherry"));

  return true;
}

bool slist_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = mutt_mem_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_create(30);

  slist_init(cs);
  if (!cs_register_variables(cs, Vars, 0))
    return false;

  cs_add_listener(cs, log_listener);

  set_list(cs);

  if (!test_initial_values(cs, &err))
    return false;

  cs_free(&cs);
  FREE(&err.data);

  return true;
}

#if 0
SLIST TESTS
3 runs (1 for each separator)?

initial:
  empty
  single token
  two tokens
  three tokens
  ":token"
  "token:"
  ":"
  "::"
initil test with "allow dupes", "allow empty"

setting - no dupes, no empty
  above tests
  "one:two:one"
  "one::true"

errors
  !allow_dupes -> quiet or fail?
  !allow_empty -> quiet or fail?

validators
reset (+validator)
getting
native get/set

slist type tests
  add_str, unique, dupe, empty list, empty string
  remove_str, missing, empty list, empty string
  cat lists, (uniq1,uniq2), empty list combos, dupe list combos
  is_member true, false, empty list, empty string

#endif
