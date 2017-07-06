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

bool config_set_init(struct ConfigSet *config, struct ConfigSet *parent)
{
  config->hash = hash_create(10, 0);
  config->parent = parent;
  return true;
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

void config_set_free(struct ConfigSet *config)
{
  hash_destroy(&config->hash, destroy);
}


bool config_set_addr(struct ConfigSet *config, const char *name, struct Address *value)
{
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_ADDR)
      return false;

    destroy(DT_ADDR, elem->data);
    elem->data = (void *) value;
    return true;
  }

  hash_typed_insert(config->hash, name, DT_ADDR, (void *) value);
  return true;
}

bool config_set_bool(struct ConfigSet *config, const char *name, bool value)
{
  intptr_t copy = value;
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_BOOL)
      return false;

    elem->data = (void *) copy;
    return true;
  }

  hash_typed_insert(config->hash, name, DT_BOOL, (void *) copy);
  return true;
}

bool config_set_hcache(struct ConfigSet *config, const char *name, const char *value)
{
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_HCACHE)
      return false;

    FREE(&elem->data);
    elem->data = (void *) value;
    return true;
  }

  hash_typed_insert(config->hash, name, DT_HCACHE, (void *) value);
  return true;
}

bool config_set_magic(struct ConfigSet *config, const char *name, int value)
{
  intptr_t copy = value;
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_MAGIC)
      return false;

    elem->data = (void *) copy;
    return true;
  }

  hash_typed_insert(config->hash, name, DT_MAGIC, (void *) copy);
  return true;
}

bool config_set_mbchartbl(struct ConfigSet *config, const char *name, struct MbCharTable *value)
{
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_MBCHARTBL)
      return false;

    destroy(DT_MBCHARTBL, elem->data);
    elem->data = (void *) value;
    return true;
  }

  hash_typed_insert(config->hash, name, DT_MBCHARTBL, (void *) value);
  return true;
}

bool config_set_num(struct ConfigSet *config, const char *name, int value)
{
  intptr_t copy = value;
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_NUM)
      return false;

    elem->data = (void *) copy;
    return true;
  }

  hash_typed_insert(config->hash, name, DT_NUM, (void *) copy);
  return true;
}

bool config_set_path(struct ConfigSet *config, const char *name, const char *value)
{
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_PATH)
      return false;

    FREE(&elem->data);
    elem->data = (void *) value;
    return true;
  }

  hash_typed_insert(config->hash, name, DT_PATH, (void *) value);
  return true;
}

bool config_set_quad(struct ConfigSet *config, const char *name, int value)
{
  intptr_t copy = value;
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_QUAD)
      return false;

    elem->data = (void *) copy;
    return true;
  }

  hash_typed_insert(config->hash, name, DT_QUAD, (void *) copy);
  return true;
}

bool config_set_rx(struct ConfigSet *config, const char *name, struct Regex *value)
{
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_RX)
      return false;

    destroy(DT_RX, elem->data);
    elem->data = (void *) value;
    return true;
  }

  hash_typed_insert(config->hash, name, DT_RX, (void *) value);
  return true;
}

bool config_set_sort(struct ConfigSet *config, const char *name, int value)
{
  intptr_t copy = value;
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_SORT)
      return false;

    elem->data = (void *) copy;
    return true;
  }

  hash_typed_insert(config->hash, name, DT_SORT, (void *) copy);
  return true;
}

bool config_set_str(struct ConfigSet *config, const char *name, const char *value)
{
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_STR)
      return false;

    FREE(&elem->data);
    elem->data = (void *) value;
    return true;
  }

  hash_typed_insert(config->hash, name, DT_STR, (void *) value);
  return true;
}


struct HashElem *find_hash(struct ConfigSet *config, const char *name)
{
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
    return elem;
  if (config->parent)
    return find_hash(config->parent, name);
  return NULL;
}

struct Address *config_get_addr(struct ConfigSet *config, const char *name)
{
  struct HashElem *elem = find_hash(config, name);
  if (elem && (DTYPE(elem->type) == DT_ADDR))
    return elem->data;
  return NULL;
}

bool config_get_bool(struct ConfigSet *config, const char *name)
{
  struct HashElem *elem = find_hash(config, name);
  if (elem && (DTYPE(elem->type) == DT_BOOL))
    return ((intptr_t) elem->data != 0);
  return false;
}

const char *config_get_hcache(struct ConfigSet *config, const char *name)
{
  struct HashElem *elem = find_hash(config, name);
  if (elem && (DTYPE(elem->type) == DT_HCACHE))
    return elem->data;
  return NULL;
}

int config_get_magic(struct ConfigSet *config, const char *name)
{
  struct HashElem *elem = find_hash(config, name);
  if (elem && (DTYPE(elem->type) == DT_MAGIC))
    return (intptr_t) elem->data;
  return -1;
}

struct MbCharTable *config_get_mbchartbl(struct ConfigSet *config, const char *name)
{
  struct HashElem *elem = find_hash(config, name);
  if (elem && (DTYPE(elem->type) == DT_MBCHARTBL))
    return elem->data;
  return NULL;
}

int config_get_num(struct ConfigSet *config, const char *name)
{
  struct HashElem *elem = find_hash(config, name);
  if (elem && (DTYPE(elem->type) == DT_NUM))
    return (intptr_t) elem->data;
  return INT_MIN;
}

const char *config_get_path(struct ConfigSet *config, const char *name)
{
  struct HashElem *elem = find_hash(config, name);
  if (elem && (DTYPE(elem->type) == DT_PATH))
    return elem->data;
  return NULL;
}

int config_get_quad(struct ConfigSet *config, const char *name)
{
  struct HashElem *elem = find_hash(config, name);
  if (elem && (DTYPE(elem->type) == DT_QUAD))
    return (intptr_t) elem->data;
  return -1;
}

struct Regex *config_get_rx(struct ConfigSet *config, const char *name)
{
  struct HashElem *elem = find_hash(config, name);
  if (elem && (DTYPE(elem->type) == DT_RX))
    return elem->data;
  return NULL;
}

int config_get_sort(struct ConfigSet *config, const char *name)
{
  struct HashElem *elem = find_hash(config, name);
  if (elem && (DTYPE(elem->type) == DT_SORT))
    return (intptr_t) elem->data;
  return -1;
}

const char *config_get_str(struct ConfigSet *config, const char *name)
{
  struct HashElem *elem = find_hash(config, name);
  if (elem && (DTYPE(elem->type) == DT_STR))
    return elem->data;
  return NULL;
}

