/**
 * @file
 * General purpose object for storing and parsing strings
 *
 * @authors
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

#ifndef _LIB_BUFFER_H
#define _LIB_BUFFER_H

#include <stddef.h>

/**
 * struct Buffer - String manipulation buffer
 */
struct Buffer
{
  char *data;   /**< pointer to data */
  char *dptr;   /**< current read/write position */
  size_t dsize; /**< length of data */
  int destroy;  /**< destroy 'data' when done? */
};

struct Buffer *mutt_buffer_new(void);
struct Buffer *mutt_buffer_init(struct Buffer *b);
void mutt_buffer_reset(struct Buffer *b);
struct Buffer *mutt_buffer_from(char *seed);
void mutt_buffer_free(struct Buffer **p);
int mutt_buffer_printf(struct Buffer *buf, const char *fmt, ...);
void mutt_buffer_addstr(struct Buffer *buf, const char *s);
void mutt_buffer_addch(struct Buffer *buf, char c);

#endif /* _LIB_BUFFER_H */
