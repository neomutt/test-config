/**
 * @file
 * NotMuch virtual mailbox type
 *
 * @authors
 * Copyright (C) 2016 Bernard Pratz <z+mutt+pub@m0g.net>
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

#ifndef _MUTT_OPTIONS_H
#define _MUTT_OPTIONS_H

#include <stddef.h>

struct Buffer;

/* supported mailbox formats */
enum
{
  MUTT_MBOX = 1,
  MUTT_MMDF,
  MUTT_MH,
  MUTT_MAILDIR,
  MUTT_NNTP,
  MUTT_IMAP,
  MUTT_NOTMUCH,
  MUTT_POP,
  MUTT_COMPRESSED,
};

/* possible arguments to set_quadoption() */
enum
{
  MUTT_ABORT = -1,
  MUTT_NO,
  MUTT_YES,
  MUTT_ASKNO,
  MUTT_ASKYES
};

/**
 * struct Option - Definition of a user-variable
 */
struct Option
{
  const char *option; /**< user-visible name */
  short type;         /**< variable type, e.g. *DT_STR */
  unsigned long data; /**< pointer to the global variable */
  unsigned long init; /**< initial value */
};

int mutt_option_to_string(const struct Option *opt, char *val, size_t len);
const char *type_to_string(int type);

const struct Option *mutt_option_get(const char *s);
int mutt_option_set(const struct Option *val, struct Buffer *err);

#endif /* _MUTT_OPTIONS_H */
