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
 * Dump all the config items in various formats.
 *
 * | Function         | Description
 * | :--------------- | :-----------------------------------------------
 * | dump_config()    | Write all the config to stdout
 * | elem_list_sort() | Sort two HashElem pointers to config
 * | escape_char()    | Write an escaped character to a buffer
 * | escape_string()  | Write a string to a buffer, escaping special characters
 * | get_elem_list()  | Create a sorted list of all config items
 * | pretty_var()     | Escape and stringify a config item value
 */

#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mutt/buffer.h"
#include "mutt/hash.h"
#include "mutt/memory.h"
#include "mutt/string2.h"
#include "dump.h"
#include "set.h"
#include "types.h"

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
 * @retval num Number of bytes written to buffer
 */
size_t escape_string(char *buf, size_t buflen, const char *src)
{
  char *p = buf;

  if (buflen == 0)
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
 * pretty_var - Escape and stringify a config item value
 * @param buf    Buffer to write to
 * @param str    String to escape
 * @retval num Number of bytes written to buffer
 */
size_t pretty_var(struct Buffer *buf, const char *str)
{
  if (!buf || !str)
    return 0;

  int len = 0;

  mutt_buffer_addch(buf, '"');
  len += escape_string(buf->dptr, buf->dsize - (buf->dptr - buf->data + 1), str);
  buf->dptr += len;
  //QWQ check for success/buffer full
  mutt_buffer_addch(buf, '"');

  return len + 2;
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

  struct HashElem *he = NULL;
  while ((he = mutt_hash_walk(cs->hash, &walk)))
    list[index++] = he;
  //XXX not range checked

  qsort(list, index, sizeof(struct HashElem *), elem_list_sort);

  return list;
}

void dump_config_mutt(struct ConfigSet *cs, struct HashElem *he, struct Buffer *value, struct Buffer *initial, int flags)
{
  const char *name = he->key.strkey;

  if (DTYPE(he->type) == DT_BOOL)
  {
    if (value->data[0] == 'y')
      printf("%s is set\n", name);
    else
      printf("%s is unset\n", name);
  }
  else
  {
    printf("%s=%s\n", name, value->data);
  }
}

void dump_config_neo(struct ConfigSet *cs, struct HashElem *he, struct Buffer *value, struct Buffer *initial, int flags)
{
  const char *name = he->key.strkey;

  if ((flags & CS_DUMP_ONLY_CHANGED) && (mutt_str_strcmp(value->data, initial->data) == 0))
    return;

  if (he->type == DT_SYNONYM)
  {
    const struct ConfigDef *cdef = he->data;
    const char *syn = (const char *) cdef->initial;
    printf("# synonym: %s -> %s\n", name, syn);
    return;
  }

  bool show_name = !(flags & CS_DUMP_HIDE_NAME);
  bool show_value = !(flags & CS_DUMP_HIDE_VALUE);

  if (show_name && show_value)
    printf("set ");
  if (show_name)
    printf("%s", name);
  if (show_name && show_value)
    printf(" = ");
  if (show_value)
    printf("%s", value->data);
  if (show_name || show_value)
    printf("\n");

  if (flags & CS_DUMP_SHOW_DEFAULTS)
  {
    const struct ConfigSetType *cst = cs_get_type_def(cs, he->type);
    printf("# %s %s %s\n", cst->name, name, value->data);
  }
}

/**
 * dump_config - Write all the config to stdout
 * @param cs    ConfigSet to dump
 * @param style Output style, e.g. #CS_DUMP_STYLE_MUTT
 * @param flags Display flags, e.g. #CS_DUMP_CHANGED
 */
bool dump_config(struct ConfigSet *cs, int style, int flags)
{
  if (!cs)
    return false;

  struct HashElem *he = NULL;

  struct Buffer buf;
  buf.data = mutt_mem_malloc(1024);
  buf.dptr = buf.data;
  buf.dsize = 0;

  struct HashElem **list = get_elem_list(cs);
  if (!list)
    return false;

  struct Buffer value;
  value.data = mutt_mem_malloc(1024);
  value.dptr = value.data;
  value.dsize = 1024;

  struct Buffer initial;
  initial.data = mutt_mem_malloc(1024);
  initial.dptr = initial.data;
  initial.dsize = 1024;

  struct Buffer tmp;
  tmp.data = mutt_mem_malloc(1024);
  tmp.dptr = tmp.data;
  tmp.dsize = 1024;

  for (size_t i = 0; list[i]; i++)
  {
    mutt_buffer_reset(&value);
    mutt_buffer_reset(&initial);
    he = list[i];

    if ((he->type == DT_SYNONYM) && !(flags & CS_DUMP_SHOW_SYNONYMS))
      continue;

    if ((he->type == DT_DISABLED) && !(flags & CS_DUMP_SHOW_DISABLED))
      continue;

    // const char *name = he->key.strkey;
    int type = he->type;
    int rc;

    if (he->type != DT_SYNONYM)
    {
      // get value
      if ((flags & CS_DUMP_ONLY_CHANGED) || !(flags & CS_DUMP_HIDE_VALUE) || (flags & CS_DUMP_SHOW_DEFAULTS))
      {
        rc = cs_he_string_get(cs, he, &value);
        if (CSR_RESULT(rc) != CSR_SUCCESS)
          return false;

        const struct ConfigDef *cdef = he->data;
        //QWQ mutt_buffer_empty()
        if (IS_SENSITIVE(*cdef) && (value.data[0] != '\0'))
        {
          mutt_buffer_reset(&value);
          mutt_buffer_addstr(&value, "***");
        }

        if ((type != DT_BOOL) && (type != DT_NUMBER) && (type != DT_QUAD) && !(flags & CS_DUMP_NO_ESCAPING))
        {
          mutt_buffer_reset(&tmp);
          size_t len = pretty_var(&tmp, value.data);
          mutt_str_strfcpy(value.data, tmp.data, len + 1);
          //QWQ mutt_buffer_copy?
        }
      }

      // get default
      if (flags & (CS_DUMP_ONLY_CHANGED || CS_DUMP_SHOW_DEFAULTS))
      {
        rc = cs_he_default_get(cs, he, &initial);
        if (CSR_RESULT(rc) != CSR_SUCCESS)
          return false;

        if ((type != DT_BOOL) && (type != DT_NUMBER) && (type != DT_QUAD) && !(flags & CS_DUMP_NO_ESCAPING))
        {
          mutt_buffer_reset(&tmp);
          size_t len = pretty_var(&tmp, initial.data);
          mutt_str_strfcpy(value.data, tmp.data, len + 1);
          //QWQ mutt_buffer_copy?
        }
      }
    }

    if (style == CS_DUMP_STYLE_MUTT)
      dump_config_mutt(cs, he, &value, &initial, flags);
    else
      dump_config_neo(cs, he, &value, &initial, flags);
  }

  FREE(&list);
  FREE(&buf.data);
  //QWQ mutt_buffer_free_static
  FREE(&value.data);
  FREE(&initial.data);
  FREE(&tmp.data);

  return true;
}
