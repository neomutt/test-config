/**
 * @file
 * Representation of an email
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

#ifndef MUTT_EMAIL_EMAIL_H
#define MUTT_EMAIL_EMAIL_H

#include "config.h"
#include <stddef.h>
#include <stdbool.h>
#include <time.h>
#include "mutt/mutt.h"
#include "ncrypt/ncrypt.h"
#include "tags.h"

/**
 * struct Email - The envelope/body of an email
 */
struct Email
{
  SecurityFlags security;   /**< bit 0-8: flags, bit 9,10: application.
                                 see: ncrypt/ncrypt.h pgplib.h, smime.h */

  bool mime            : 1; /**< has a MIME-Version header? */
  bool flagged         : 1; /**< marked important? */
  bool tagged          : 1;
  bool deleted         : 1;
  bool purge           : 1; /**< skip trash folder when deleting */
  bool quasi_deleted   : 1; /**< deleted from neomutt, but not modified on disk */
  bool changed         : 1;
  bool attach_del      : 1; /**< has an attachment marked for deletion */
  bool old             : 1;
  bool read            : 1;
  bool expired         : 1; /**< already expired? */
  bool superseded      : 1; /**< got superseded? */
  bool replied         : 1;
  bool subject_changed : 1; /**< used for threading */
  bool threaded        : 1; /**< used for threading */
  bool display_subject : 1; /**< used for threading */
  bool recip_valid     : 1; /**< is_recipient is valid */
  bool active          : 1; /**< message is not to be removed */
  bool trash           : 1; /**< message is marked as trashed on disk.
                             * This flag is used by the maildir_trash option. */

  /* timezone of the sender of this message */
  unsigned int zhours : 5;
  unsigned int zminutes : 6;
  bool zoccident : 1;

  /* bits used for caching when searching */
  bool searched : 1;
  bool matched : 1;

  /* tells whether the attachment count is valid */
  bool attach_valid : 1;

  /* the following are used to support collapsing threads  */
  bool collapsed : 1; /**< is this message part of a collapsed thread? */
  bool limited : 1;   /**< is this message in a limited view?  */
  size_t num_hidden;  /**< number of hidden messages in this view */

  short recipient;    /**< user_is_recipient()'s return value, cached */

  int pair;           /**< color-pair to use when displaying in the index */

  time_t date_sent;   /**< time when the message was sent (UTC) */
  time_t received;    /**< time when the message was placed in the mailbox */
  LOFF_T offset;      /**< where in the stream does this message begin? */
  int lines;          /**< how many lines in the body of this message? */
  int index;          /**< the absolute (unsorted) message number */
  int msgno;          /**< number displayed to the user */
  int virtual;        /**< virtual message number */
  int score;
  struct Envelope *env;      /**< envelope information */
  struct Body *content;      /**< list of MIME parts */
  char *path;

  char *tree; /**< character string to print thread tree */
  struct MuttThread *thread;

  /* Number of qualifying attachments in message, if attach_valid */
  short attach_total;

#ifdef MIXMASTER
  struct ListHead chain;
#endif

#ifdef USE_POP
  int refno; /**< message number on server */
#endif

  struct TagHead tags; /**< for drivers that support server tagging */

  char *maildir_flags; /**< unknown maildir flags */

  void *edata;                 /**< driver-specific data */
  void (*free_edata)(void **); /**< driver-specific data free function */
};

/**
 * struct EmailNode - List of Emails
 */
struct EmailNode
{
  struct Email *email;
  STAILQ_ENTRY(EmailNode) entries;
};
STAILQ_HEAD(EmailList, EmailNode);

bool          mutt_email_cmp_strict(const struct Email *e1, const struct Email *e2);
void          mutt_email_free(struct Email **e);
struct Email *mutt_email_new(void);
size_t        mutt_email_size(const struct Email *e);

void el_free(struct EmailList *el);

#endif /* MUTT_EMAIL_EMAIL_H */
