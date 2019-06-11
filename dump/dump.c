/**
 * @file
 * Test Code for config dumping
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

#include <stdbool.h>
#include <stdio.h>
#include "mutt/buffer.h"
#include "mutt/memory.h"
#include "mutt/string2.h"
#include "config/lib.h"
#include "data.h"
#include "test/common.h"

void config_dump(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = mutt_mem_calloc(1, 256);
  err.dsize = 256;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new(500);

  address_init(cs);
  bool_init(cs);
  enum_init(cs);
  long_init(cs);
  mbtable_init(cs);
  number_init(cs);
  quad_init(cs);
  regex_init(cs);
  slist_init(cs);
  sort_init(cs);
  string_init(cs);

  if (!cs_register_variables(cs, MuttVars, 0))
    return;

  notify_observer_add(cs->notify, NT_CONFIG, 0, log_observer, 0);

  dump_config(cs, CS_DUMP_HIDE_SENSITIVE | CS_DUMP_SHOW_DEFAULTS | CS_DUMP_SHOW_SYNONYMS, stdout);
  printf("\n");

  dump_config(cs, CS_DUMP_ONLY_CHANGED, stdout);
  printf("\n");

  cs_free(&cs);
  FREE(&err.data);
  log_line(__func__);
}
