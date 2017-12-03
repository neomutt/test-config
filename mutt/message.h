/**
 * @file
 * Message logging
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

#ifndef _MUTT_MESSAGE_H
#define _MUTT_MESSAGE_H

#ifdef ENABLE_NLS
#include <libintl.h>
#define _(a) gettext(a)
#ifdef gettext_noop
#define N_(a) gettext_noop(a)
#else
#define N_(a) (a)
#endif
#else
#define _(a) (a)
#define N_(a) a
#endif

void (*mutt_error)  (const char *format, ...);
void (*mutt_message)(const char *format, ...);
void (*mutt_perror) (const char *message);

#endif /* _MUTT_MESSAGE_H */
