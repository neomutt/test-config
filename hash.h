/**
 * Copyright (C) 1996-2009 Michael R. Elkins <me@mutt.org>
 *
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

#ifndef _MUTT_HASH_H
#define _MUTT_HASH_H 1

#include <stdbool.h>

union hash_key {
  const char *strkey;
  unsigned int intkey;
};

struct HashElem
{
  int type;
  union hash_key key;
  void *data;
  struct HashElem *next;
};

struct Hash
{
  int nelem, curnelem;
  bool strdup_keys : 1; /* if set, the key->strkey is strdup'ed */
  bool allow_dups  : 1; /* if set, duplicate keys are allowed */
  struct HashElem **table;
  unsigned int (*gen_hash)(union hash_key, unsigned int);
  int (*cmp_key)(union hash_key, union hash_key);
};

/* flags for hash_create() */
#define MUTT_HASH_STRCASECMP  (1 << 0) /* use strcasecmp() to compare keys */
#define MUTT_HASH_STRDUP_KEYS (1 << 1) /* make a copy of the keys */
#define MUTT_HASH_ALLOW_DUPS  (1 << 2) /* allow duplicate keys to be inserted */

struct Hash *hash_create(int nelem, int flags);
struct Hash *int_hash_create(int nelem, int flags);

struct HashElem *hash_typed_insert(struct Hash *table, const char *strkey, int type, void *data);
struct HashElem *hash_insert(struct Hash *table, const char *strkey, void *data);
struct HashElem *int_hash_insert(struct Hash *table, unsigned int intkey, void *data);
struct Hash *hash_resize(struct Hash *ptr, int nelem, int lower);

void *hash_find(const struct Hash *table, const char *strkey);
struct HashElem *hash_find_elem(const struct Hash *table, const char *strkey);
void *int_hash_find(const struct Hash *table, unsigned int intkey);

struct HashElem *hash_find_bucket(const struct Hash *table, const char *strkey);

void hash_delete(struct Hash *table, const char *strkey, const void *data,
                 void (*destroy)(int type, void *obj));
void int_hash_delete(struct Hash *table, unsigned int intkey, const void *data,
                     void (*destroy)(int type, void *obj));

void hash_destroy(struct Hash **ptr, void (*destroy)(int type, void *obj));

struct HashWalkState
{
  int index;
  struct HashElem *last;
};

struct HashElem *hash_walk(const struct Hash *table, struct HashWalkState *state);

#endif /* _MUTT_HASH_H */
