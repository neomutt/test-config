/**
 * @file
 * Representation of a mailbox
 *
 * @authors
 * Copyright (C) 1996-2000,2010,2013 Michael R. Elkins <me@mutt.org>
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

#ifndef MUTT_ACCOUNT_H
#define MUTT_ACCOUNT_H

#include "mutt/queue.h"
#include "config/lib.h"
#include "mailbox.h"

struct ConnAccount;

/**
 * struct Account - Set of Mailboxes, e.g. An Imap server
 *
 * Named Accounts can also be used to define sets of Maildir mailboxes.
 */
struct Account
{
  enum MailboxType magic;
  struct MailboxList mailboxes;
  TAILQ_ENTRY(Account) entries;
  void *adata;
  void (*free_adata)(void **);

  char *name;                 ///< Name of Account
  const struct ConfigSet *cs; ///< Parent ConfigSet
  const char **var_names;     ///< Array of the names of local config items
  size_t num_vars;            ///< Number of local config items
  struct HashElem **vars;     ///< Array of the HashElems of local config items
};
TAILQ_HEAD(AccountList, Account);

extern struct AccountList AllAccounts; ///< List of all Accounts
extern struct AccountList CurrentConfigAccount; ///< Current 'account' command in use

bool            account_add_config(struct Account *a, const struct ConfigSet *cs, const char *name, const char *var_names[]);
struct Account *account_find(const char *name);
void            account_free(struct Account **ptr);
void            account_free_config(struct Account *a);
char *          account_get_current(void);
int             account_get_value(const struct Account *a, size_t vid, struct Buffer *result);
struct Account *account_new(struct ConfigSet *cs, const char *name);
void            account_pop_current(void);
void            account_push_current(char *name);
void            account_remove_mailbox(struct Account *a, struct Mailbox *m);
int             account_set_value(const struct Account *a, size_t vid, intptr_t value, struct Buffer *err);

#endif /* MUTT_ACCOUNT_H */
