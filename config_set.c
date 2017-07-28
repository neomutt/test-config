#include "config.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "config_set.h"
#include "hash.h"
#include "lib.h"
#include "mutt_options.h"

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
  struct HashElem *e = NULL;
  struct HashWalkState state;
  memset(&state, 0, sizeof(state));

  struct Buffer result;
  mutt_buffer_init(&result);
  result.data = calloc(1, STRING);
  result.dsize = STRING;

  while ((e = hash_walk(set->hash, &state)))
  {
    struct ConfigSetType *type = get_type_def(e->type);
    mutt_buffer_reset(&result);
    printf("%s [%s]", e->key.strkey, type->name);

    if (type && type->getter(e, &result))
      printf(" = %s\n", result.data);
    else
      printf(": ERROR: %s\n", result.data);
  }

  FREE(&result.data);
}

bool cs_register_type(int type_id, struct ConfigSetType *cst)
{
  RegisteredTypes[type_id] = *cst;
  return false;
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

