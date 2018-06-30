/**
 * @file
 * Test code for the Command object
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
#include "config/command.h"
#include "config/set.h"
#include "config/types.h"
#include "test/common.h"

static short VarApple;

// clang-format off
static struct ConfigDef Vars[] = {
  { "Apple", DT_COMMAND, 0, &VarApple, 0, NULL },
  { NULL },
};
// clang-format on

static bool test_initial_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  return true;
}

bool command_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = mutt_mem_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_create(30);

  command_init(cs);
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
