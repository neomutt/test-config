/**
 * @file
 * Hash table data structure
 *
 * @authors
 * Copyright (C) 1996-2009 Michael R. Elkins <me@mutt.org>
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

#ifndef _LIB_HASH_H
#define _LIB_HASH_H

#include <stdbool.h>
#include <stdint.h>

/**
 * union HashKey - The data item stored in a HashElem
 */
union HashKey {
  const char *strkey;
  unsigned int intkey;
};

/**
 * struct HashElem - The item stored in a Hash Table
 */
struct HashElem
{
  int type;
  union HashKey key;
  void *data;
  struct HashElem *next;
};

typedef void (*hash_destructor)(int type, void *obj, intptr_t data);

/**
 * struct Hash - A Hash Table
 */
struct Hash
{
  int nelem;
  bool strdup_keys : 1; /**< if set, the key->strkey is strdup'ed */
  bool allow_dups  : 1; /**< if set, duplicate keys are allowed */
  struct HashElem **table;
  unsigned int (*gen_hash)(union HashKey, unsigned int);
  int (*cmp_key)(union HashKey, union HashKey);
  hash_destructor destroy;
  intptr_t dest_data;
};

/* flags for hash_create() */
#define MUTT_HASH_STRCASECMP  (1 << 0) /**< use strcasecmp() to compare keys */
#define MUTT_HASH_STRDUP_KEYS (1 << 1) /**< make a copy of the keys */
#define MUTT_HASH_ALLOW_DUPS  (1 << 2) /**< allow duplicate keys to be inserted */

struct Hash *hash_create(int nelem, int flags);
struct Hash *int_hash_create(int nelem, int flags);
void hash_set_destructor(struct Hash *hash, hash_destructor fn, intptr_t fn_data);

struct HashElem *hash_typed_insert(struct Hash *table, const char *strkey, int type, void *data);
struct HashElem *hash_insert(struct Hash *table, const char *strkey, void *data);
struct HashElem *int_hash_insert(struct Hash *table, unsigned int intkey, void *data);

void *hash_find(const struct Hash *table, const char *strkey);
struct HashElem *hash_find_elem(const struct Hash *table, const char *strkey);
void *int_hash_find(const struct Hash *table, unsigned int intkey);

struct HashElem *hash_find_bucket(const struct Hash *table, const char *strkey);

void hash_delete(struct Hash *table, const char *strkey, const void *data);
void int_hash_delete(struct Hash *table, unsigned int intkey, const void *data);
void hash_destroy(struct Hash **ptr);

void hash_dump(struct Hash *table);

/**
 * struct HashWalkState - Cursor to iterate through a Hash Table
 */
struct HashWalkState
{
  int index;
  struct HashElem *last;
};

struct HashElem *hash_walk(const struct Hash *table, struct HashWalkState *state);

#endif /* _LIB_HASH_H */
