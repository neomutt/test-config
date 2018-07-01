/**
 * @file
 * Type representing a list of strings
 *
 * @authors
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
 * @page config-slist Type: List of strings
 *
 * Type representing a list of strings.
 */

#include "config.h"
#include <stddef.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>
#include "mutt/buffer.h"
#include "mutt/logging.h"
#include "mutt/memory.h"
#include "mutt/queue.h"
#include "mutt/string2.h"
#include "slist.h"
#include "set.h"
#include "types.h"

/**
 * slist_destroy - Destroy an Slist object
 * @param cs   Config items
 * @param var  Variable to destroy
 * @param cdef Variable definition
 */
static void slist_destroy(const struct ConfigSet *cs, void *var, const struct ConfigDef *cdef)
{
  if (!cs || !var || !cdef)
    return; /* LCOV_EXCL_LINE */

  struct Slist **l = (struct Slist **) var;
  if (!*l)
    return;

  slist_free(l);
}

/**
 * slist_string_set - Set a Slist by string
 * @param cs    Config items
 * @param var   Variable to set
 * @param cdef  Variable definition
 * @param value Value to set
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 *
 * If var is NULL, then the config item's initial value will be set.
 */
static int slist_string_set(const struct ConfigSet *cs, void *var, struct ConfigDef *cdef,
                            const char *value, struct Buffer *err)
{
  if (!cs || !cdef || !value)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  if (value && (value[0] == '\0'))
    value = NULL;

  struct Slist *list = NULL;

  int rc = CSR_SUCCESS;

  if (var)
  {
    list = slist_parse(value, cdef->flags);

    if (cdef->validator)
    {
      rc = cdef->validator(cs, cdef, (intptr_t) list, err);

      if (CSR_RESULT(rc) != CSR_SUCCESS)
      {
        slist_free(&list);
        return (rc | CSR_INV_VALIDATOR);
      }
    }

    slist_destroy(cs, var, cdef);

    *(struct Slist **) var = list;

    if (!list)
      rc |= CSR_SUC_EMPTY;
  }
  else
  {
    if (cdef->type & DT_INITIAL_SET)
      FREE(&cdef->initial);

    cdef->type |= DT_INITIAL_SET;
    cdef->initial = IP mutt_str_strdup(value);
  }

  return CSR_SUCCESS;
}

/**
 * slist_string_get - Get a Slist as a string
 * @param cs     Config items
 * @param var    Variable to get
 * @param cdef   Variable definition
 * @param result Buffer for results or error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 *
 * If var is NULL, then the config item's initial value will be returned.
 */
static int slist_string_get(const struct ConfigSet *cs, void *var,
                            const struct ConfigDef *cdef, struct Buffer *result)
{
  if (!cs || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  if (var)
  {
    struct Slist *list = *(struct Slist **) var;
    if (!list)
      return (CSR_SUCCESS | CSR_SUC_EMPTY); /* empty string */

    struct ListNode *np = NULL;
    STAILQ_FOREACH(np, &list->head, entries)
    {
      mutt_buffer_addstr(result, np->data);
      if (STAILQ_NEXT(np, entries))
      {
        int sep = (cdef->flags & SLIST_SEP_MASK);
        if (sep == SLIST_SEP_COMMA)
          mutt_buffer_addch(result, ',');
        else if (sep == SLIST_SEP_COLON)
          mutt_buffer_addch(result, ':');
        else
          mutt_buffer_addch(result, ' ');
      }
    }
  }
  else
  {
    mutt_buffer_addstr(result, (char *) cdef->initial);
  }

  int rc = CSR_SUCCESS;
  if (mutt_buffer_is_empty(result))
    rc |= CSR_SUC_EMPTY;

  return rc;
}

/**
 * slist_native_set - Set a Slist config item by STAILQ
 * @param cs    Config items
 * @param var   Variable to set
 * @param cdef  Variable definition
 * @param value Mailbox slist
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int slist_native_set(const struct ConfigSet *cs, void *var,
                            const struct ConfigDef *cdef, intptr_t value, struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  int rc;

  if (cdef->validator)
  {
    rc = cdef->validator(cs, cdef, value, err);

    if (CSR_RESULT(rc) != CSR_SUCCESS)
      return (rc | CSR_INV_VALIDATOR);
  }

  slist_free(var);

  struct Slist *list = slist_dup((struct Slist *) value);

  rc = CSR_SUCCESS;
  if (!list)
    rc |= CSR_SUC_EMPTY;

  *(struct Slist **) var = list;
  return rc;
}

/**
 * slist_native_get - Get a STAILQ from a Slist config item
 * @param cs   Config items
 * @param var  Variable to get
 * @param cdef Variable definition
 * @param err  Buffer for error messages
 * @retval intptr_t Mailbox slist
 */
static intptr_t slist_native_get(const struct ConfigSet *cs, void *var,
                                 const struct ConfigDef *cdef, struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  return *(short *) var;
}

/**
 * slist_reset - Reset a Slist to its initial value
 * @param cs   Config items
 * @param var  Variable to reset
 * @param cdef Variable definition
 * @param err  Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
static int slist_reset(const struct ConfigSet *cs, void *var,
                       const struct ConfigDef *cdef, struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct Slist *list = NULL;
  const char *initial = (const char *) cdef->initial;

  struct Slist *curlist = *(struct Slist **) var;

  int rc = CSR_SUCCESS;
  if (!curlist)
    rc |= CSR_SUC_EMPTY;

  if (initial)
    list = slist_parse(initial, cdef->flags);

  if (cdef->validator)
  {
    rc = cdef->validator(cs, cdef, (intptr_t) list, err);

    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      slist_destroy(cs, &list, cdef);
      return (rc | CSR_INV_VALIDATOR);
    }
  }

  if (!list)
    rc |= CSR_SUC_EMPTY;

  slist_destroy(cs, var, cdef);

  *(struct Slist **) var = list;
  return rc;
}

/**
 * slist_init - Register the Slist config type
 * @param cs Config items
 */
void slist_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_slist = {
    "slist",          slist_string_set, slist_string_get, slist_native_set,
    slist_native_get, slist_reset,      slist_destroy,
  };
  cs_register_type(cs, DT_SLIST, &cst_slist);
}

/**
 * slist_add_list - Add a list to another list
 * @param list String list to add to
 * @param add  String list to add
 * @retval ptr Modified list
 */
struct Slist *slist_add_list(struct Slist *list, const struct Slist *add)
{
  if (!add)
    return list;
  if (!list)
    return slist_dup(add);

  struct ListNode *np = NULL;
  STAILQ_FOREACH(np, &add->head, entries)
  {
    mutt_list_insert_tail(&list->head, mutt_str_strdup((char *) np->data));
    list->count++;
  }
  return list;
}

/**
 * slist_add_string - Add a string to a list
 * @param list List to modify
 * @param str  String to add
 * @retval ptr Modified list
 */
struct Slist *slist_add_string(struct Slist *list, const char *str)
{
  if (!list)
    return NULL;

  if (str && (str[0] == '\0'))
    str = NULL;

  if (!str && !(list->flags & SLIST_ALLOW_EMPTY))
    return list;

  mutt_list_insert_tail(&list->head, mutt_str_strdup(str));
  list->count++;

  return list;
}

/**
 * slist_compare - Compare two string lists
 * @param a First list
 * @param b Second list
 * @retval true If they are identical
 */
bool slist_compare(const struct Slist *a, const struct Slist *b)
{
  if (!a && !b) /* both empty */
    return true;
  if (!a ^ !b) /* one is empty, but not the other */
    return false;
  if (a->count != b->count)
    return false;

  return false;
}

/**
 * slist_dup - Create a copy of an Slist object
 * @param list Slist to duplicate
 * @retval ptr New Slist object
 */
struct Slist *slist_dup(const struct Slist *list)
{
  if (!list)
    return NULL; /* LCOV_EXCL_LINE */

  struct Slist *l = mutt_mem_calloc(1, sizeof(*l));
  l->flags = list->flags;
  STAILQ_INIT(&l->head);

  struct ListNode *np = NULL;
  STAILQ_FOREACH(np, &list->head, entries)
  {
    mutt_list_insert_tail(&l->head, mutt_str_strdup(np->data));
  }
  return l;
}

/**
 * slist_empty - Empty out an Slist object
 * @param list Slist to duplicate
 * @retval ptr New Slist object
 */
struct Slist *slist_empty(struct Slist **list)
{
  if (!list || !*list)
    return NULL; /* LCOV_EXCL_LINE */

  mutt_list_free(&(*list)->head);

  if ((*list)->flags & SLIST_ALLOW_EMPTY)
  {
    (*list)->count = 0;
    return *list;
  }

  FREE(list);
  return NULL;
}

/**
 * slist_free - Free an Slist object
 * @param list Slist to free
 */
void slist_free(struct Slist **list)
{
  if (!list || !*list)
    return; /* LCOV_EXCL_LINE */

  mutt_list_free(&(*list)->head);
  FREE(list);
}

/**
 * slist_is_member - Is a string a member of a list?
 * @param list List to modify
 * @param str  String to find
 * @param true String is in the list
 */
bool slist_is_member(const struct Slist *list, const char *str)
{
  if (!list)
    return false;

  if (!str && !(list->flags & SLIST_ALLOW_EMPTY))
    return false;

  struct ListNode *np = NULL;
  STAILQ_FOREACH(np, &list->head, entries)
  {
    if (mutt_str_strcmp(np->data, str) == 0)
      return true;
  }
  return false;
}

/**
 * slist_parse - Parse a list of strings into a list
 * @param s String of strings
 * @retval ptr New Slist object
 */
struct Slist *slist_parse(const char *str, int flags)
{
  char *src = mutt_str_strdup(str);
  if (!src && !(flags & SLIST_ALLOW_EMPTY))
    return NULL;

  char sep = ' ';
  if ((flags & SLIST_SEP_MASK) == SLIST_SEP_COMMA)
    sep = ',';
  else if ((flags & SLIST_SEP_MASK) == SLIST_SEP_COLON)
    sep = ':';

  struct Slist *list = mutt_mem_calloc(1, sizeof(struct Slist));
  list->flags = flags;
  STAILQ_INIT(&list->head);

  if (!src)
    return list;

  char *start = src;
  for (char *p = start; *p; p++)
  {
    if ((p[0] == '\\') && (p[1] != '\0'))
    {
      p++;
      continue;
    }

    if (p[0] == sep)
    {
      p[0] = '\0';
      mutt_list_insert_tail(&list->head, mutt_str_strdup(start));
      list->count++;
      start = p + 1;
    }
  }

  mutt_list_insert_tail(&list->head, mutt_str_strdup(start));
  list->count++;

  FREE(&src);
  return list;
}

/**
 * slist_remove_string - Remove a string from a list
 * @param list List to modify
 * @param str  String to remove
 * @retval ptr Modified list
 */
struct Slist *slist_remove_string(struct Slist *list, const char *str)
{
  if (!list)
    return NULL;
  if (!str && !(list->flags & SLIST_ALLOW_EMPTY))
    return list;

  struct ListNode *prev = NULL;
  struct ListNode *np = NULL;
  struct ListNode *tmp = NULL;
  STAILQ_FOREACH_SAFE(np, &list->head, entries, tmp)
  {
    if (mutt_str_strcmp(np->data, str) == 0)
    {
      if (prev)
        STAILQ_REMOVE_AFTER(&list->head, prev, entries);
      else
        STAILQ_REMOVE_HEAD(&list->head, entries);
      FREE(&np->data);
      FREE(&np);
      list->count--;
      break;
    }
    prev = np;
  }
  return list;
}
