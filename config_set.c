#include "config.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "config_set.h"
#include "hash.h"
#include "lib.h"
#include "mutt_options.h"
#include "type/address.h"
#include "type/mbyte_table.h"
#include "type/mutt_regex.h"

struct ConfigSetType RegisteredTypes[16] =
{
  { NULL, NULL, NULL, },
};


struct ConfigSetType *get_type_def(int type)
{
  type = DTYPE(type);
  if ((type < 0) || (type >= mutt_array_size(RegisteredTypes)))
    return NULL;

  return &RegisteredTypes[type];
}

static void destroy(int type, void *obj, intptr_t data)
{
  // struct ConfigSet *set = (struct ConfigSet *) data;

  struct VariableDef *def = obj;

  struct ConfigSetType *cs = get_type_def(type);
  if (cs->destructor)
    cs->destructor(def->variable);
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

void cs_dump_set(struct ConfigSet *set)
{
  struct HashElem *entry = NULL;
  struct HashWalkState state;
  memset(&state, 0, sizeof(state));

  while ((entry = hash_walk(set->hash, &state)))
  {
    printf("%s ", entry->key.strkey);
  }
  printf("\n");
}


bool cs_register_type(const char *name, int type_id, struct ConfigSetType *cst)
{
  RegisteredTypes[type_id] = *cst;
  return false;
}

struct HashElem *cs_set_addr(struct ConfigSet *set, const char *name, struct Address *value, struct Buffer *result)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = (intptr_t) value;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem && (DTYPE(elem->type) != DT_ADDR))
  {
    mutt_buffer_printf(result, "Variable is type %s, trying to use type %s", type_to_string(DTYPE(elem->type)), type_to_string(DT_ADDR));
    return NULL;
  }

  if (set->validator && !set->validator(set, name, DT_ADDR, copy, NULL))
    return NULL;

  if (elem)
  {
    // if (set->destructor && !set->destructor(set, DT_ADDR, (intptr_t) elem->data))
    //   FREE(&elem->data);
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

struct HashElem *cs_set_bool(struct ConfigSet *set, const char *name, bool value, struct Buffer *result)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = value;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem && (DTYPE(elem->type) != DT_BOOL))
  {
    mutt_buffer_printf(result, "Variable is type %s, trying to use type %s", type_to_string(DTYPE(elem->type)), type_to_string(DT_BOOL));
    return NULL;
  }

  if (set->validator && !set->validator(set, name, DT_BOOL, copy, NULL))
    return NULL;

  if (elem)
  {
    // if (set->destructor)
    //   set->destructor(set, DT_BOOL, (intptr_t) elem->data);
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

struct HashElem *cs_set_hcache(struct ConfigSet *set, const char *name, const char *value, struct Buffer *result)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = (intptr_t) value;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem && (DTYPE(elem->type) != DT_HCACHE))
  {
    mutt_buffer_printf(result, "Variable is type %s, trying to use type %s", type_to_string(DTYPE(elem->type)), type_to_string(DT_HCACHE));
    return NULL;
  }

  if (set->validator && !set->validator(set, name, DT_HCACHE, copy, NULL))
    return NULL;

  if (elem)
  {
    // if (set->destructor && !set->destructor(set, DT_HCACHE, (intptr_t) elem->data))
    //   FREE(&elem->data);
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

struct HashElem *cs_set_magic(struct ConfigSet *set, const char *name, int value, struct Buffer *result)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = value;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem && (DTYPE(elem->type) != DT_MAGIC))
  {
    mutt_buffer_printf(result, "Variable is type %s, trying to use type %s", type_to_string(DTYPE(elem->type)), type_to_string(DT_MAGIC));
    return NULL;
  }

  if (set->validator && !set->validator(set, name, DT_MAGIC, copy, NULL))
    return NULL;

  if (elem)
  {
    // if (set->destructor)
    //   set->destructor(set, DT_MAGIC, (intptr_t) elem->data);
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

struct HashElem *cs_set_mbchartbl(struct ConfigSet *set, const char *name, struct MbCharTable *value, struct Buffer *result)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = (intptr_t) value;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem && (DTYPE(elem->type) != DT_MBCHARTBL))
  {
    mutt_buffer_printf(result, "Variable is type %s, trying to use type %s", type_to_string(DTYPE(elem->type)), type_to_string(DT_MBCHARTBL));
    return NULL;
  }

  if (set->validator && !set->validator(set, name, DT_MBCHARTBL, copy, NULL))
    return NULL;

  if (elem)
  {
    // if (set->destructor && !set->destructor(set, DT_MBCHARTBL, (intptr_t) elem->data))
    //   FREE(&elem->data);
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

struct HashElem *cs_set_num(struct ConfigSet *set, const char *name, int value, struct Buffer *result)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = value;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem && (DTYPE(elem->type) != DT_NUM))
  {
    mutt_buffer_printf(result, "Variable is type %s, trying to use type %s", type_to_string(DTYPE(elem->type)), type_to_string(DT_NUM));
    return NULL;
  }

  if (set->validator && !set->validator(set, name, DT_NUM, copy, result))
    return NULL;

  if (elem)
  {
    // if (set->destructor)
    //   set->destructor(set, DT_NUM, (intptr_t) elem->data);
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

struct HashElem *cs_set_path(struct ConfigSet *set, const char *name, const char *value, struct Buffer *result)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = (intptr_t) value;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem && (DTYPE(elem->type) != DT_PATH))
  {
    mutt_buffer_printf(result, "Variable is type %s, trying to use type %s", type_to_string(DTYPE(elem->type)), type_to_string(DT_PATH));
    return NULL;
  }

  if (set->validator && !set->validator(set, name, DT_PATH, copy, NULL))
    return NULL;

  if (elem)
  {
    // if (set->destructor && !set->destructor(set, DT_PATH, (intptr_t) elem->data))
    //   FREE(&elem->data);
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

struct HashElem *cs_set_quad(struct ConfigSet *set, const char *name, int value, struct Buffer *result)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = value;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem && (DTYPE(elem->type) != DT_QUAD))
  {
    mutt_buffer_printf(result, "Variable is type %s, trying to use type %s", type_to_string(DTYPE(elem->type)), type_to_string(DT_QUAD));
    return NULL;
  }

  if (set->validator && !set->validator(set, name, DT_QUAD, copy, NULL))
    return NULL;

  if (elem)
  {
    // if (set->destructor)
    //   set->destructor(set, DT_QUAD, (intptr_t) elem->data);
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

struct HashElem *cs_set_rx(struct ConfigSet *set, const char *name, struct Regex *value, struct Buffer *result)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = (intptr_t) value;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem && (DTYPE(elem->type) != DT_RX))
  {
    mutt_buffer_printf(result, "Variable is type %s, trying to use type %s", type_to_string(DTYPE(elem->type)), type_to_string(DT_RX));
    return NULL;
  }

  if (set->validator && !set->validator(set, name, DT_RX, copy, NULL))
    return NULL;

  if (elem)
  {
    // if (set->destructor && !set->destructor(set, DT_RX, (intptr_t) elem->data))
    //   FREE(&elem->data);
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

struct HashElem *cs_set_sort(struct ConfigSet *set, const char *name, int value, struct Buffer *result)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = value;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem && (DTYPE(elem->type) != DT_SORT))
  {
    mutt_buffer_printf(result, "Variable is type %s, trying to use type %s", type_to_string(DTYPE(elem->type)), type_to_string(DT_SORT));
    return NULL;
  }

  if (set->validator && !set->validator(set, name, DT_SORT, copy, NULL))
    return NULL;

  if (elem)
  {
    // if (set->destructor)
    //   set->destructor(set, DT_SORT, (intptr_t) elem->data);
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

struct HashElem *cs_set_str(struct ConfigSet *set, const char *name, const char *value, struct Buffer *result)
{
  enum ConfigEvent e = CE_CHANGED;
  intptr_t copy = (intptr_t) value;

  struct HashElem *elem = hash_find_elem(set->hash, name);
  if (elem && (DTYPE(elem->type) != DT_STR))
  {
    mutt_buffer_printf(result, "Variable is type %s, trying to use type %s", type_to_string(DTYPE(elem->type)), type_to_string(DT_STR));
    return NULL;
  }

  if (set->validator && !set->validator(set, name, DT_STR, copy, NULL))
    return NULL;

  if (elem)
  {
    // if (set->destructor && !set->destructor(set, DT_STR, (intptr_t) elem->data))
    //   FREE(&elem->data);
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

static struct HashElem *reg_one_var(struct ConfigSet *set, struct VariableDef *var, struct Buffer *err)
{
  struct ConfigSetType *type = get_type_def(var->type);
  if (!type)
  {
    mutt_buffer_printf(err, "Variable '%s' has an invalid type %d", var->name, var->type);
    return NULL;
  }

  struct HashElem *e = hash_typed_insert(set->hash, var->name, var->type, var);
  if (type->resetter)
  {
    if (!type->resetter(set, e, err))
    {
      hash_delete(set->hash, var->name, (void*) var->variable);
      return NULL;
    }
  }
  else
  {
    //XXX var->variable = var->initial;
  }

  return e;
}

bool cs_register_variables(struct ConfigSet *set, struct VariableDef vars[])
{
  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = calloc(1, STRING);
  err.dsize = STRING;

  for (int i = 0; vars[i].name; i++)
  {
    reg_one_var(set, &vars[i], &err);
    // printf("register: %s\n", vars[i].name);
  }

  FREE(&err.data);
  return true;
}

