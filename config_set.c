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

static void destroy(int type, void *obj, intptr_t data)
{
  struct ConfigSet *set = (struct ConfigSet *) data;
  if (set->destructor && set->destructor(set, type, (intptr_t) obj))
    return;

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

struct ConfigSet *cs_set_new(struct ConfigSet *parent)
{
  struct ConfigSet *cs = safe_calloc(1, sizeof(*cs));
  cs_init(cs, parent);
  return cs;
}

bool cs_init(struct ConfigSet *set, struct ConfigSet *parent)
{
  memset(set, 0, sizeof(*set));
  set->hash = hash_create(10, 0);
  hash_set_destructor(set->hash, destroy, (intptr_t) set);
  set->parent = parent;
  return true;
}

void cs_add_listener(struct ConfigSet *set, cs_listener fn)
{
  for (unsigned int i = 0; i < mutt_array_size(set->listeners); i++)
  {
    if (!set->listeners[i])
    {
      set->listeners[i] = fn;
      break;
    }
  }
}

void cs_add_validator(struct ConfigSet *set, cs_validator fn)
{
  set->validator = fn;
}

void cs_add_destructor(struct ConfigSet *set, cs_destructor fn)
{
  set->destructor = fn;
}

void cs_free(struct ConfigSet *set)
{
  hash_destroy(&set->hash);
}

void notify_listeners(struct ConfigSet *set, const char *name, enum ConfigEvent e)
{
  for (unsigned int i = 0; i < mutt_array_size(set->listeners); i++)
  {
    if (!set->listeners[i])
      return;

    set->listeners[i](set, name, e);
  }
}


struct HashElem *cs_set_addr(struct ConfigSet *set, const char *name, struct Address *value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = (intptr_t) value;

  if (set->validator && !set->validator(set, name, DT_ADDR, copy))
    return NULL;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_ADDR)
      return NULL;

    if (set->destructor && !set->destructor(set, DT_ADDR, (intptr_t) elem->data))
      FREE(&elem->data);
    elem->data = (void *) value;
  }
  else
  {
    e = CE_SET;
    elem = hash_typed_insert(set->hash, name, DT_ADDR, (void *) value);
  }

  notify_listeners(set, name, e);
  return elem;
}

struct HashElem *cs_set_bool(struct ConfigSet *set, const char *name, bool value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = value;

  if (set->validator && !set->validator(set, name, DT_BOOL, copy))
    return NULL;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_BOOL)
      return NULL;

    if (set->destructor)
      set->destructor(set, DT_BOOL, (intptr_t) elem->data);
    elem->data = (void *) copy;
  }
  else
  {
    e = CE_SET;
    elem = hash_typed_insert(set->hash, name, DT_BOOL, (void *) copy);
  }

  notify_listeners(set, name, e);
  return elem;
}

struct HashElem *cs_set_hcache(struct ConfigSet *set, const char *name, const char *value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = (intptr_t) value;

  if (set->validator && !set->validator(set, name, DT_HCACHE, copy))
    return NULL;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_HCACHE)
      return NULL;

    if (set->destructor && !set->destructor(set, DT_HCACHE, (intptr_t) elem->data))
      FREE(&elem->data);
    elem->data = (void *) value;
  }
  else
  {
    e = CE_SET;
    elem = hash_typed_insert(set->hash, name, DT_HCACHE, (void *) value);
  }

  notify_listeners(set, name, e);
  return elem;
}

struct HashElem *cs_set_magic(struct ConfigSet *set, const char *name, int value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = value;

  if (set->validator && !set->validator(set, name, DT_MAGIC, copy))
    return NULL;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_MAGIC)
      return NULL;

    if (set->destructor)
      set->destructor(set, DT_MAGIC, (intptr_t) elem->data);
    elem->data = (void *) copy;
  }
  else
  {
    e = CE_SET;
    elem = hash_typed_insert(set->hash, name, DT_MAGIC, (void *) copy);
  }

  notify_listeners(set, name, e);
  return elem;
}

struct HashElem *cs_set_mbchartbl(struct ConfigSet *set, const char *name, struct MbCharTable *value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = (intptr_t) value;

  if (set->validator && !set->validator(set, name, DT_MBCHARTBL, copy))
    return NULL;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_MBCHARTBL)
      return NULL;

    if (set->destructor && !set->destructor(set, DT_MBCHARTBL, (intptr_t) elem->data))
      FREE(&elem->data);
    elem->data = (void *) value;
  }
  else
  {
    e = CE_SET;
    elem = hash_typed_insert(set->hash, name, DT_MBCHARTBL, (void *) value);
  }

  notify_listeners(set, name, e);
  return elem;
}

struct HashElem *cs_set_num(struct ConfigSet *set, const char *name, int value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = value;

  if (set->validator && !set->validator(set, name, DT_NUM, copy))
    return NULL;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_NUM)
      return NULL;

    if (set->destructor)
      set->destructor(set, DT_NUM, (intptr_t) elem->data);
    elem->data = (void *) copy;
  }
  else
  {
    e = CE_SET;
    elem = hash_typed_insert(set->hash, name, DT_NUM, (void *) copy);
  }

  notify_listeners(set, name, e);
  return elem;
}

struct HashElem *cs_set_path(struct ConfigSet *set, const char *name, const char *value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = (intptr_t) value;

  if (set->validator && !set->validator(set, name, DT_PATH, copy))
    return NULL;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_PATH)
      return NULL;

    if (set->destructor && !set->destructor(set, DT_PATH, (intptr_t) elem->data))
      FREE(&elem->data);
    elem->data = (void *) value;
  }
  else
  {
    e = CE_SET;
    elem = hash_typed_insert(set->hash, name, DT_PATH, (void *) value);
  }

  notify_listeners(set, name, e);
  return elem;
}

struct HashElem *cs_set_quad(struct ConfigSet *set, const char *name, int value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = value;

  if (set->validator && !set->validator(set, name, DT_QUAD, copy))
    return NULL;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_QUAD)
      return NULL;

    if (set->destructor)
      set->destructor(set, DT_QUAD, (intptr_t) elem->data);
    elem->data = (void *) copy;
  }
  else
  {
    e = CE_SET;
    elem = hash_typed_insert(set->hash, name, DT_QUAD, (void *) copy);
  }

  notify_listeners(set, name, e);
  return elem;
}

struct HashElem *cs_set_rx(struct ConfigSet *set, const char *name, struct Regex *value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = (intptr_t) value;

  if (set->validator && !set->validator(set, name, DT_RX, copy))
    return NULL;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_RX)
      return NULL;

    if (set->destructor && !set->destructor(set, DT_RX, (intptr_t) elem->data))
      FREE(&elem->data);
    elem->data = (void *) value;
  }
  else
  {
    e = CE_SET;
    elem = hash_typed_insert(set->hash, name, DT_RX, (void *) value);
  }

  notify_listeners(set, name, e);
  return elem;
}

struct HashElem *cs_set_sort(struct ConfigSet *set, const char *name, int value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = value;

  if (set->validator && !set->validator(set, name, DT_SORT, copy))
    return NULL;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_SORT)
      return NULL;

    if (set->destructor)
      set->destructor(set, DT_SORT, (intptr_t) elem->data);
    elem->data = (void *) copy;
  }
  else
  {
    e = CE_SET;
    elem = hash_typed_insert(set->hash, name, DT_SORT, (void *) copy);
  }

  notify_listeners(set, name, e);
  return elem;
}

struct HashElem *cs_set_str(struct ConfigSet *set, const char *name, const char *value)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = (intptr_t) value;

  if (set->validator && !set->validator(set, name, DT_STR, copy))
    return NULL;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
  {
    if (DTYPE(elem->type) != DT_STR)
      return NULL;

    if (set->destructor && !set->destructor(set, DT_STR, (intptr_t) elem->data))
      FREE(&elem->data);
    elem->data = (void *) value;
  }
  else
  {
    e = CE_SET;
    elem = hash_typed_insert(set->hash, name, DT_STR, (void *) value);
  }

  notify_listeners(set, name, e);
  return elem;
}


struct HashElem *cs_get_elem(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem)
    return elem;
  if (set->parent)
    return cs_get_elem(set->parent, name);
  return NULL;
}

struct Address *cs_get_addr(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = cs_get_elem(set, name);
  if (elem && (DTYPE(elem->type) == DT_ADDR))
    return elem->data;
  return NULL;
}

bool cs_get_bool(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = cs_get_elem(set, name);
  if (elem && (DTYPE(elem->type) == DT_BOOL))
    return ((intptr_t)(elem->data != 0));
  return false;
}

const char *cs_get_hcache(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = cs_get_elem(set, name);
  if (elem && (DTYPE(elem->type) == DT_HCACHE))
    return elem->data;
  return NULL;
}

int cs_get_magic(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = cs_get_elem(set, name);
  if (elem && (DTYPE(elem->type) == DT_MAGIC))
    return (intptr_t) elem->data;
  return -1;
}

struct MbCharTable *cs_get_mbchartbl(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = cs_get_elem(set, name);
  if (elem && (DTYPE(elem->type) == DT_MBCHARTBL))
    return elem->data;
  return NULL;
}

int cs_get_num(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = cs_get_elem(set, name);
  if (elem && (DTYPE(elem->type) == DT_NUM))
    return (intptr_t) elem->data;
  return INT_MIN;
}

const char *cs_get_path(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = cs_get_elem(set, name);
  if (elem && (DTYPE(elem->type) == DT_PATH))
    return elem->data;
  return NULL;
}

int cs_get_quad(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = cs_get_elem(set, name);
  if (elem && (DTYPE(elem->type) == DT_QUAD))
    return (intptr_t) elem->data;
  return -1;
}

struct Regex *cs_get_rx(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = cs_get_elem(set, name);
  if (elem && (DTYPE(elem->type) == DT_RX))
    return elem->data;
  return NULL;
}

int cs_get_sort(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = cs_get_elem(set, name);
  if (elem && (DTYPE(elem->type) == DT_SORT))
    return (intptr_t) elem->data;
  return -1;
}

const char *cs_get_str(struct ConfigSet *set, const char *name)
{
  struct HashElem *elem = cs_get_elem(set, name);
  if (elem && (DTYPE(elem->type) == DT_STR))
    return elem->data;
  return NULL;
}


struct HashElem *he_set_addr(struct HashElem *var, struct Address *value)
{
  if (!var)
    return NULL;

  if (DTYPE(var->type) != DT_ADDR)
    return NULL;

  destroy(DT_ADDR, var->data, 0);
  var->data = (void *) value;

  // notify_listeners(set, name, e);
  return var;
}

struct HashElem *he_set_bool(struct HashElem *var, bool value)
{
  if (!var)
    return NULL;

  if (DTYPE(var->type) != DT_BOOL)
    return NULL;

  intptr_t copy = value;
  var->data = (void *) copy;

  // notify_listeners(set, name, e);
  return var;
}

struct HashElem *he_set_hcache(struct HashElem *var, const char *value)
{
  if (!var)
    return NULL;

  if (DTYPE(var->type) != DT_HCACHE)
    return NULL;

  FREE(&var->data);
  var->data = (void *) value;

  // notify_listeners(set, name, e);
  return var;
}

struct HashElem *he_set_magic(struct HashElem *var, int value)
{
  if (!var)
    return NULL;

  if (DTYPE(var->type) != DT_MAGIC)
    return NULL;

  intptr_t copy = value;
  var->data = (void *) copy;

  // notify_listeners(set, name, e);
  return var;
}

struct HashElem *he_set_mbchartbl(struct HashElem *var, struct MbCharTable *value)
{
  if (!var)
    return NULL;

  if (DTYPE(var->type) != DT_MBCHARTBL)
    return NULL;

  destroy(DT_MBCHARTBL, var->data, 0);
  var->data = (void *) value;

  // notify_listeners(set, name, e);
  return var;
}

struct HashElem *he_set_num(struct HashElem *var, int value)
{
  if (!var)
    return NULL;

  if (DTYPE(var->type) != DT_NUM)
    return NULL;

  intptr_t copy = value;
  var->data = (void *) copy;

  // notify_listeners(set, name, e);
  return var;
}

struct HashElem *he_set_path(struct HashElem *var, const char *value)
{
  if (!var)
    return NULL;

  if (DTYPE(var->type) != DT_PATH)
    return NULL;

  FREE(&var->data);
  var->data = (void *) value;

  // notify_listeners(set, name, e);
  return var;
}

struct HashElem *he_set_quad(struct HashElem *var, int value)
{
  if (!var)
    return NULL;

  if (DTYPE(var->type) != DT_QUAD)
    return NULL;

  intptr_t copy = value;
  var->data = (void *) copy;

  // notify_listeners(set, name, e);
  return var;
}

struct HashElem *he_set_rx(struct HashElem *var, struct Regex *value)
{
  if (!var)
    return NULL;

  if (DTYPE(var->type) != DT_RX)
    return NULL;

  destroy(DT_RX, var->data, 0);
  var->data = (void *) value;

  // notify_listeners(set, name, e);
  return var;
}

struct HashElem *he_set_sort(struct HashElem *var, int value)
{
  if (!var)
    return NULL;

  if (DTYPE(var->type) != DT_SORT)
    return NULL;

  intptr_t copy = value;
  var->data = (void *) copy;

  // notify_listeners(set, name, e);
  return var;
}

struct HashElem *he_set_str(struct HashElem *var, const char *value)
{
  if (!var)
    return NULL;

  if (DTYPE(var->type) != DT_STR)
    return NULL;

  FREE(&var->data);
  var->data = (void *) value;

  // notify_listeners(set, name, e);
  return var;
}


struct Address *he_get_addr(struct HashElem *var)
{
  if (var && (DTYPE(var->type) == DT_ADDR))
    return var->data;
  return NULL;
}

bool he_get_bool(struct HashElem *var)
{
  if (var && (DTYPE(var->type) == DT_BOOL))
    return ((intptr_t)(var->data != 0));
  return false;
}

const char *he_get_hcache(struct HashElem *var)
{
  if (var && (DTYPE(var->type) == DT_HCACHE))
    return var->data;
  return NULL;
}

int he_get_magic(struct HashElem *var)
{
  if (var && (DTYPE(var->type) == DT_MAGIC))
    return (intptr_t) var->data;
  return -1;
}

struct MbCharTable *he_get_mbchartbl(struct HashElem *var)
{
  if (var && (DTYPE(var->type) == DT_MBCHARTBL))
    return var->data;
  return NULL;
}

int he_get_num(struct HashElem *var)
{
  if (var && (DTYPE(var->type) == DT_NUM))
    return (intptr_t) var->data;
  return INT_MIN;
}

const char *he_get_path(struct HashElem *var)
{
  if (var && (DTYPE(var->type) == DT_PATH))
    return var->data;
  return NULL;
}

int he_get_quad(struct HashElem *var)
{
  if (var && (DTYPE(var->type) == DT_QUAD))
    return (intptr_t) var->data;
  return -1;
}

struct Regex *he_get_rx(struct HashElem *var)
{
  if (var && (DTYPE(var->type) == DT_RX))
    return var->data;
  return NULL;
}

int he_get_sort(struct HashElem *var)
{
  if (var && (DTYPE(var->type) == DT_SORT))
    return (intptr_t) var->data;
  return -1;
}

const char *he_get_str(struct HashElem *var)
{
  if (var && (DTYPE(var->type) == DT_STR))
    return var->data;
  return NULL;
}

