#include "config.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "config_set.h"
#include "address.h"
#include "hash.h"
#include "lib.h"
#include "mbyte_table.h"
#include "mutt_options.h"
#include "mutt_regex.h"

struct ConfigSet *config_set_new(struct ConfigSet *parent)
{
  struct ConfigSet *cs = safe_calloc(1, sizeof(*cs));
  config_set_init(cs, parent);
  return cs;
}

bool config_set_init(struct ConfigSet *set, struct ConfigSet *parent)
{
  set->hash = hash_create(10, 0);
  set->parent = parent;
  memset(set->cb, 0, sizeof(set->cb));
  return true;
}

void config_set_add_callback(struct ConfigSet *set, config_callback cb)
{
  for (unsigned int i = 0; i < mutt_array_size(set->cb); i++)
  {
    if (!set->cb[i])
    {
      set->cb[i] = cb;
      break;
    }
  }
}

void destroy(int type, void *obj)
{
  switch (type)
  {
    case DT_BOOL:
    case DT_MAGIC:
    case DT_NUM:
    case DT_QUAD:
    case DT_SORT:
      break;

    case DT_ADDR:
    {
      struct Address *a = obj;
      FREE(&a->personal);
      FREE(&a->mailbox);
      FREE(&a);
      break;
    }

    case DT_MBCHARTBL:
    {
      struct MbCharTable *m = obj;
      FREE(&m->segmented_str);
      FREE(&m->orig_str);
      FREE(&m);
      break;
    }

    case DT_RX:
    {
      struct Regex *r = obj;
      FREE(&r->pattern);
      //regfree(r->rx)
      FREE(&r);
    }
    break;

    case DT_HCACHE:
    case DT_PATH:
    case DT_STR:
      free(obj);
      break;
  }
}

void config_set_free(struct ConfigSet *set)
{
  hash_destroy(&set->hash, destroy);
}


void notify_listeners(struct ConfigSet *set, const char *name, enum ConfigEvent e)
{
  for (unsigned int i = 0; i < mutt_array_size(set->cb); i++)
  {
    if (!set->cb[i])
      return;

    set->cb[i](set, name, e);
  }
}

bool config_set_addr(struct ConfigSet *set, const char *name, struct Address *value)
{
  enum ConfigEvent e = CE_CHANGED;
  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_ADDR)
      return false;

    destroy(DT_ADDR, elem->data);
    elem->data = (void *) value;
  }
  else
  {
    e = CE_SET;
    hash_typed_insert(set->hash, name, DT_ADDR, (void *) value);
  }

  notify_listeners(set, name, e);
  return true;
}

bool config_set_bool(struct ConfigSet *set, const char *name, bool value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = value;
  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_BOOL)
      return false;

    elem->data = (void *) copy;
  }
  else
  {
    e = CE_SET;
    hash_typed_insert(set->hash, name, DT_BOOL, (void *) copy);
  }

  notify_listeners(set, name, e);
  return true;
}

bool config_set_hcache(struct ConfigSet *set, const char *name, const char *value)
{
  enum ConfigEvent e = CE_CHANGED;
  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_HCACHE)
      return false;

    FREE(&elem->data);
    elem->data = (void *) value;
  }
  else
  {
    e = CE_SET;
    hash_typed_insert(set->hash, name, DT_HCACHE, (void *) value);
  }

  notify_listeners(set, name, e);
  return true;
}

bool config_set_magic(struct ConfigSet *set, const char *name, int value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = value;
  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_MAGIC)
      return false;

    elem->data = (void *) copy;
  }
  else
  {
    e = CE_SET;
    hash_typed_insert(set->hash, name, DT_MAGIC, (void *) copy);
  }

  notify_listeners(set, name, e);
  return true;
}

bool config_set_mbchartbl(struct ConfigSet *set, const char *name, struct MbCharTable *value)
{
  enum ConfigEvent e = CE_CHANGED;
  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_MBCHARTBL)
      return false;

    destroy(DT_MBCHARTBL, elem->data);
    elem->data = (void *) value;
  }
  else
  {
    e = CE_SET;
    hash_typed_insert(set->hash, name, DT_MBCHARTBL, (void *) value);
  }

  notify_listeners(set, name, e);
  return true;
}

bool config_set_num(struct ConfigSet *set, const char *name, int value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = value;
  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_NUM)
      return false;

    elem->data = (void *) copy;
  }
  else
  {
    e = CE_SET;
    hash_typed_insert(set->hash, name, DT_NUM, (void *) copy);
  }

  notify_listeners(set, name, e);
  return true;
}

bool config_set_path(struct ConfigSet *set, const char *name, const char *value)
{
  enum ConfigEvent e = CE_CHANGED;
  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_PATH)
      return false;

    FREE(&elem->data);
    elem->data = (void *) value;
  }
  else
  {
    e = CE_SET;
    hash_typed_insert(set->hash, name, DT_PATH, (void *) value);
  }

  notify_listeners(set, name, e);
  return true;
}

bool config_set_quad(struct ConfigSet *set, const char *name, int value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = value;
  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_QUAD)
      return false;

    elem->data = (void *) copy;
  }
  else
  {
    e = CE_SET;
    hash_typed_insert(set->hash, name, DT_QUAD, (void *) copy);
  }

  notify_listeners(set, name, e);
  return true;
}

bool config_set_rx(struct ConfigSet *set, const char *name, struct Regex *value)
{
  enum ConfigEvent e = CE_CHANGED;
  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_RX)
      return false;

    destroy(DT_RX, elem->data);
    elem->data = (void *) value;
  }
  else
  {
    e = CE_SET;
    hash_typed_insert(set->hash, name, DT_RX, (void *) value);
  }

  notify_listeners(set, name, e);
  return true;
}

bool config_set_sort(struct ConfigSet *set, const char *name, int value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = value;
  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_SORT)
      return false;

    elem->data = (void *) copy;
  }
  else
  {
    e = CE_SET;
    hash_typed_insert(set->hash, name, DT_SORT, (void *) copy);
  }

  notify_listeners(set, name, e);
  return true;
}

bool config_set_str(struct ConfigSet *set, const char *name, const char *value)
{
  enum ConfigEvent e = CE_CHANGED;
  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_STR)
      return false;

    FREE(&elem->data);
    elem->data = (void *) value;
  }
  else
  {
    e = CE_SET;
    hash_typed_insert(set->hash, name, DT_STR, (void *) value);
  }

  notify_listeners(set, name, e);
  return true;
}


struct HashElem *find_hash(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
    return elem;
  if (set->parent)
    return find_hash(set->parent, name);
  return NULL;
}

struct Address *config_get_addr(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = find_hash(set, name);
  if (elem && (DTYPE(elem->type) == DT_ADDR))
    return elem->data;
  return NULL;
}

bool config_get_bool(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = find_hash(set, name);
  if (elem && (DTYPE(elem->type) == DT_BOOL))
    return ((intptr_t) elem->data != 0);
  return false;
}

const char *config_get_hcache(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = find_hash(set, name);
  if (elem && (DTYPE(elem->type) == DT_HCACHE))
    return elem->data;
  return NULL;
}

int config_get_magic(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = find_hash(set, name);
  if (elem && (DTYPE(elem->type) == DT_MAGIC))
    return (intptr_t) elem->data;
  return -1;
}

struct MbCharTable *config_get_mbchartbl(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = find_hash(set, name);
  if (elem && (DTYPE(elem->type) == DT_MBCHARTBL))
    return elem->data;
  return NULL;
}

int config_get_num(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = find_hash(set, name);
  if (elem && (DTYPE(elem->type) == DT_NUM))
    return (intptr_t) elem->data;
  return INT_MIN;
}

const char *config_get_path(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = find_hash(set, name);
  if (elem && (DTYPE(elem->type) == DT_PATH))
    return elem->data;
  return NULL;
}

int config_get_quad(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = find_hash(set, name);
  if (elem && (DTYPE(elem->type) == DT_QUAD))
    return (intptr_t) elem->data;
  return -1;
}

struct Regex *config_get_rx(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = find_hash(set, name);
  if (elem && (DTYPE(elem->type) == DT_RX))
    return elem->data;
  return NULL;
}

int config_get_sort(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = find_hash(set, name);
  if (elem && (DTYPE(elem->type) == DT_SORT))
    return (intptr_t) elem->data;
  return -1;
}

const char *config_get_str(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = find_hash(set, name);
  if (elem && (DTYPE(elem->type) == DT_STR))
    return elem->data;
  return NULL;
}

