/**
 * @file
 * Convenience wrapper for the library headers
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

/**
 * @page mutt MUTT: Shared code for handling strings, files, dates, etc
 *
 * Each source file in the library provides a group of related functions.
 *
 * | File             | Description        |
 * | :--------------- | :----------------- |
 * | mutt/base64.c    | @subpage base64    |
 * | mutt/buffer.c    | @subpage buffer    |
 * | mutt/charset.c   | @subpage charset   |
 * | mutt/date.c      | @subpage date      |
 * | mutt/envlist.c   | @subpage envlist   |
 * | mutt/exit.c      | @subpage exit      |
 * | mutt/file.c      | @subpage file      |
 * | mutt/group.c     | @subpage group     |
 * | mutt/hash.c      | @subpage hash      |
 * | mutt/history.c   | @subpage history   |
 * | mutt/list.c      | @subpage list      |
 * | mutt/logging.c   | @subpage logging   |
 * | mutt/mapping.c   | @subpage mapping   |
 * | mutt/mbyte.c     | @subpage mbyte     |
 * | mutt/md5.c       | @subpage md5       |
 * | mutt/memory.c    | @subpage memory    |
 * | mutt/path.c      | @subpage path      |
 * | mutt/regex.c     | @subpage regex     |
 * | mutt/sha1.c      | @subpage sha1      |
 * | mutt/signal.c    | @subpage signal    |
 * | mutt/string.c    | @subpage string    |
 *
 * @note The library is self-contained -- some files may depend on others in
 *       the library, but none depends on source from outside.
 */

#ifndef MUTT_LIB_MUTT_H
#define MUTT_LIB_MUTT_H

#include "base64.h"
#include "buffer.h"
#include "charset.h"
#include "date.h"
#include "envlist.h"
#include "exit.h"
#include "file.h"
#include "group.h"
#include "hash.h"
#include "history.h"
#include "list.h"
#include "logging.h"
#include "mapping.h"
#include "mbyte.h"
#include "md5.h"
#include "memory.h"
#include "message.h"
#include "queue.h"
#include "path.h"
#include "regex3.h"
#include "sha1.h"
#include "signal2.h"
#include "string2.h"

#endif /* MUTT_LIB_MUTT_H */
