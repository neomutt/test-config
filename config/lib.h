/**
 * @file
 * Convenience wrapper for the config headers
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

/**
 * @page config CONFIG: Flexible handling of config items
 *
 * User configurable variables.
 *
 * | File                | Description                |
 * | :------------------ | :------------------------- |
 * | config/address.c    | @subpage config_address    |
 * | config/bool.c       | @subpage config_bool       |
 * | config/gaccount.c   | @subpage config_account    |
 * | config/command.c    | @subpage config_command    |
 * | config/dump.c       | @subpage config_dump       |
 * | config/long.c       | @subpage config_long       |
 * | config/magic.c      | @subpage config_magic      |
 * | config/mbtable.c    | @subpage config_mbtable    |
 * | config/number.c     | @subpage config_number     |
 * | config/path.c       | @subpage config_path       |
 * | config/quad.c       | @subpage config_quad       |
 * | config/regex.c      | @subpage config_regex      |
 * | config/set.c        | @subpage config_set        |
 * | config/sort.c       | @subpage config_sort       |
 * | config/string.c     | @subpage config_string     |
 */

#ifndef MUTT_CONFIG_LIB_H
#define MUTT_CONFIG_LIB_H

#include "account.h"
#include "address.h"
#include "bool.h"
#include "command.h"
#include "dump.h"
#include "enum.h"
#include "inheritance.h"
#include "long.h"
#include "magic.h"
#include "mbtable.h"
#include "number.h"
#include "path.h"
#include "quad.h"
#include "regex2.h"
#include "set.h"
#include "slist.h"
#include "sort.h"
#include "string3.h"
#include "types.h"

#endif /* MUTT_CONFIG_LIB_H */
