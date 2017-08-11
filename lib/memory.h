/**
 * @file
 * Memory management wrappers
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

#ifndef _LIB_MEMORY_H
#define _LIB_MEMORY_H

#include <stddef.h>

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

#define mutt_array_size(x) (sizeof(x) / sizeof((x)[0]))

void *safe_calloc(size_t nmemb, size_t size);
void  safe_free(void *ptr);
void *safe_malloc(size_t size);
void  safe_realloc(void *ptr, size_t size);

#define FREE(x) safe_free(x)

#endif /* _LIB_MEMORY_H */
