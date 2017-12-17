/**
 * @file
 * Dump all the config
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
 * @page config-dump Dump all the config
 *
 * LONG dump
 *
 * | Function                | Description
 * | :---------------------- | :-----------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mutt/buffer.h"
#include "mutt/hash.h"
#include "mutt/memory.h"
#include "mutt/string2.h"
#include "set.h"
#include "types.h"

#define F_SENSITIVE (1 << 9)
#define IS_SENSITIVE(x) (((x).flags & F_SENSITIVE) == F_SENSITIVE)

/**
 * escape_char - Write an escaped character to a buffer
 * @param buf    Buffer to write to
 * @param buflen Length of buffer
 * @param c      Character to write
 * @param p      Where to write the character (pointer into buf)
 */
void escape_char(char *buf, size_t buflen, char c, char *p)
{
  *p++ = '\\';
  if ((p - buf) < buflen)
    *p++ = c;
}

/**
 * escape_string - Write a string to a buffer, escaping special characters
 * @param buf    Buffer to write to
 * @param buflen Length of buffer
 * @param src    String to write
 * @retval num Number of bytes left in buffer
 */
size_t escape_string(char *buf, size_t buflen, const char *src)
{
  char *p = buf;

  if (!buflen)
    return 0;
  buflen--; /* save room for \0 */
  while (((p - buf) < buflen) && src && *src)
  {
    switch (*src)
    {
      case '\n':
        escape_char(buf, buflen, 'n', p);
        break;
      case '\r':
        escape_char(buf, buflen, 'r', p);
        break;
      case '\t':
        escape_char(buf, buflen, 't', p);
        break;
      default:
        if (((*src == '\\') || (*src == '"')) && ((p - buf) < (buflen - 1)))
          *p++ = '\\';
        *p++ = *src;
    }
    src++;
  }
  *p = '\0';
  return (p - buf);
}

/**
 * pretty_var - Write a config option to a buffer
 * @param buf    Buffer to write to
 * @param buflen Length of buffer
 * @param name   Name of config name
 * @param val    Value of config name
 */
void pretty_var(char *buf, size_t buflen, const char *name, const char *val)
{
  char *p = NULL;

  if (buflen == 0)
    return;

  mutt_str_strfcpy(buf, name, buflen);
  buflen--; /* save room for \0 */
  p = buf + mutt_str_strlen(buf);

  if ((p - buf) < buflen)
    *p++ = '=';
  if ((p - buf) < buflen)
    *p++ = '"';
  p += escape_string(p, buflen - (p - buf) + 1, val); /* \0 terminate it */
  if ((p - buf) < buflen)
    *p++ = '"';
  *p = '\0';
}

/**
 * elem_list_sort - Sort two HashElem pointers to config
 * @param a First HashElem
 * @param b Second HashElem
 * @retval -1 a precedes b
 * @retval  0 a and b are identical
 * @retval  1 b precedes a
 */
int elem_list_sort(const void *a, const void *b)
{
  const struct HashElem *hea = *(struct HashElem **) a;
  const struct HashElem *heb = *(struct HashElem **) b;

  return mutt_str_strcasecmp(hea->key.strkey, heb->key.strkey);
}

/**
 * get_elem_list - Create a sorted list of all config items
 * @param cs ConfigSet to read
 * @retval ptr Null-terminated array of HashElem
 */
struct HashElem **get_elem_list(struct ConfigSet *cs)
{
  if (!cs)
    return NULL;

  struct HashElem **list = mutt_mem_calloc(1024, sizeof(struct HashElem *));
  size_t index = 0;

  struct HashWalkState walk;
  memset(&walk, 0, sizeof(walk));

  struct HashElem *elem = NULL;
  while ((elem = mutt_hash_walk(cs->hash, &walk)))
    list[index++] = elem;
  //XXX not range checked

  qsort(list, index, sizeof(struct HashElem *), elem_list_sort);

  return list;
}

/**
 * dump_config - Write all the config to stdout
 * @param cs    ConfigSet to dump
 * @param style Output style, e.g. #CS_DUMP_STYLE_MUTT
 * @param flags Display flags, e.g. #CS_DUMP_CHANGED
 */
void dump_config(struct ConfigSet *cs, int style, int flags)
{
  if (!cs)
    return;

  char disp[1024];
  struct HashElem *elem = NULL;
  bool hide_sensitive = true;
  bool defaults = true;

  struct Buffer buf;
  buf.data = mutt_mem_malloc(1024);
  buf.dptr = buf.data;
  buf.dsize = 0;

  struct HashElem **list = get_elem_list(cs);
  if (!list)
    return;

  for (size_t i = 0; list[i]; i++)
  {
    elem = list[i];
    if (elem->type == DT_SYNONYM)
      continue;

    const struct ConfigDef *cdef = elem->data;
    if (hide_sensitive && IS_SENSITIVE(*cdef))
    {
      printf("%s='***'\n", elem->key.strkey);
      continue;
    }

    mutt_buffer_reset(&buf);
    cs_he_string_get(cs, elem, &buf);

    if (defaults)
    {
      char b[1024];
      struct Buffer def = { b, b, sizeof(b), 0 };
      cs_he_default_get(cs, elem, &def);
      char esc[1024];
      escape_string(esc, sizeof(esc), def.data);
      const struct ConfigSetType *cst = NULL;
      cst = cs_get_type_def(cs, elem->type);
      printf("# %s\t%s\t%s\n", elem->key.strkey, cst->name, esc);
    }

    pretty_var(disp, sizeof(disp), elem->key.strkey, buf.data);
    printf("%s\n", disp);
  }

  FREE(&list);
  FREE(&buf.data);
}
