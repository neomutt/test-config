#include "config_set.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/lib.h"
#include "data.h"
#include "mutt_options.h"
#include "inheritance.h"

struct ConfigSetType RegisteredTypes[16] =
{
  { NULL, NULL, NULL, },
};


const int ConfigSetSize = 10;// 500;

struct ConfigSetType *get_type_def(unsigned int type)
{
  type = DTYPE(type);
  if ((type < 1) || (type >= mutt_array_size(RegisteredTypes)))
    return NULL;

  return &RegisteredTypes[type];
}

static void destroy(int type, void *obj, intptr_t data)
{
  // struct ConfigSet *set = (struct ConfigSet *) data;

  if (type & DT_INHERITED)
  {
    struct Inheritance *i = obj;
    FREE(&i->name);
    FREE(&i);
    return;
  }

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
  set->hash = hash_create(ConfigSetSize, 0);
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
    const char *name = NULL;

    if (e->type & DT_INHERITED)
    {
      struct Inheritance *i = e->data;
      e = i->parent;
      name = i->name;
    }
    else
    {
      name = e->key.strkey;
    }

    struct ConfigSetType *type = get_type_def(e->type);
    if (!type)
    {
      printf("Unknown type: %d\n", e->type);
      continue;
    }

    mutt_buffer_reset(&result);
    printf("%-7s %s", type->name, name);

    if (type->getter(e, &result))
      printf(" = %s\n", result.data);
    else
      printf(": ERROR: %s\n", result.data);
  }

  FREE(&result.data);
}

bool cs_register_type(unsigned int type, struct ConfigSetType *cst)
{
  RegisteredTypes[type] = *cst;
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

bool cs_set_variable(struct ConfigSet *set, const char *name, const char *value, struct Buffer *err)
{
  struct HashElem *e = hash_find_elem(set->hash, name);
  if (!e)
  {
    mutt_buffer_printf(err, "Unknown variable '%s'", name);
    return false;
  }

  struct ConfigSetType *cst = NULL;

  if (e->type & DT_INHERITED)
  {
    struct Inheritance *i = e->data;
    cst = get_type_def(i->parent->type);
  }
  else
  {
    cst = get_type_def(e->type);
  }

  if (!cst)
  {
    mutt_buffer_printf(err, "Variable '%s' has an invalid type %d", name, e->type);
    return false;
  }

  if (!cst->setter)
  {
    mutt_buffer_printf(err, "No setter for '%s'", name);
    return false;
  }

  void *variable = NULL;

  if (e->type & DT_INHERITED)
  {
    struct Inheritance *i = e->data;
    variable = &i->var;
  }
  else
  {
    struct VariableDef *v = e->data;
    variable = v->variable;
  }

  if (!variable)
    return false;

  if (!cst->setter(set, variable, value, err))
    return false;

  if (e->type & DT_INHERITED)
  {
    struct Inheritance *i = e->data;
    e->type = i->parent->type | DT_INHERITED;
  }
  notify_listeners(set, name, CE_SET);
  return true;
}

bool cs_get_variable(struct ConfigSet *set, const char *name, struct Buffer *result)
{
  if (!set || !name)
    return false;

  struct HashElem *e = hash_find_elem(set->hash, name);
  if (!e)
  {
    mutt_buffer_printf(result, "Unknown variable '%s'", name);
    return false;
  }

  void *variable = NULL;
  int type = 0;

  if (e->type & DT_INHERITED)
  {
    struct Inheritance *i = e->data;

    if (DTYPE(e->type) == 0)
    {
      // Delegate to parent
      e = i->parent;

      type = DTYPE(e->type);

      struct VariableDef *v = e->data;
      if (!v)
        return false;

      variable = v->variable;
    }
    else
    {
      // Local value
      type = DTYPE(e->type);
      variable = &i->var;
    }
  }
  else
  {
    // Normal variable
    type = DTYPE(e->type);

    struct VariableDef *v = e->data;
    if (!v)
      return false;

    variable = v->variable;
  }

  struct ConfigSetType *cst = get_type_def(type);
  if (!cst)
  {
    mutt_buffer_printf(result, "Variable '%s' has an invalid type %d", name, type);
    return false;
  }

  if (!cst->getter)
  {
    mutt_buffer_printf(result, "No getter for '%s'", name);
    return false;
  }

  if (!cst->getter(variable, result))
    return false;

  return true;
}

struct HashElem *cs_get_elem(struct ConfigSet *set, const char *name)
{
  if (!set || !name)
    return NULL;

  struct HashElem *he = hash_find_elem(set->hash, name);

  if (he && (he->type & DT_INHERITED))
  {
    struct Inheritance *i = he->data;
    he = i->parent;
  }

  return he;
}

struct HashElem *cs_inherit_variable(struct ConfigSet *set, struct HashElem *parent, const char *name)
{
  if (!set || !parent)
    return NULL;

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = calloc(1, STRING);
  err.dsize = STRING;

  struct ConfigSetType *type = get_type_def(parent->type);
  if (!type)
  {
    mutt_buffer_printf(&err, "Variable '%s' has an invalid type %d", name, parent->type);
    return NULL;
  }

  struct Inheritance *i = safe_calloc(1, sizeof(*i));
  i->parent = parent;
  i->name = safe_strdup(name);

  struct HashElem *e = hash_typed_insert(set->hash, i->name, DT_INHERITED, i);

  FREE(&err.data);
  return e;
}

