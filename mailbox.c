/**
 * @file
 * Representation of a mailbox
 *
 * @authors
 * Copyright (C) 1996-2000,2010,2013 Michael R. Elkins <me@mutt.org>
 * Copyright (C) 2016-2017 Kevin J. McCarthy <kevin@8t8.us>
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

/**
 * @page mailbox Representation of a mailbox
 *
 * Representation of a mailbox
 */

#include "config.h"
#include <dirent.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <utime.h>
#include "mutt/mutt.h"
#include "config/lib.h"
#include "email/lib.h"
#include "mutt.h"
#include "mailbox.h"
#include "account.h"
#include "context.h"
#include "globals.h"
#include "maildir/lib.h"
#include "mbox/mbox.h"
#include "mutt_commands.h"
#include "mutt_menu.h"
#include "mutt_window.h"
#include "muttlib.h"
#include "mx.h"
#include "protos.h"

struct MailboxList AllMailboxes = STAILQ_HEAD_INITIALIZER(AllMailboxes);

/**
 * mailbox_new - Create a new Mailbox
 * @retval ptr New Mailbox
 */
struct Mailbox *mailbox_new(void)
{
  struct Mailbox *m = mutt_mem_calloc(1, sizeof(struct Mailbox));

  m->pathbuf = mutt_buffer_new();

  return m;
}

/**
 * mailbox_free - Free a Mailbox
 * @param[out] ptr Mailbox to free
 */
void mailbox_free(struct Mailbox **ptr)
{
  if (!ptr || !*ptr)
    return;

  struct Mailbox *m = *ptr;

  mutt_buffer_free(&m->pathbuf);
  FREE(&m->desc);
  if (m->mdata && m->free_mdata)
    m->free_mdata(&m->mdata);
  FREE(&m->realpath);
  FREE(ptr);
}

