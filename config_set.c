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

struct ConfigSetType *get_type_def(unsigned int type)
{
  type = DTYPE(type);
  if ((type < 1) || (type >= mutt_array_size(RegisteredTypes)))
    return NULL;

  return &RegisteredTypes[type];
}

static void destroy(int type, void *obj, intptr_t data)
{
  // struct ConfigSet *cs = (struct ConfigSet *) data;

  struct ConfigSetType *cst = NULL;

  if (type & DT_INHERITED)
  {
    struct Inheritance *i = obj;
    // struct VariableDef *vdef = i->parent->data;
    // cst = get_type_def(i->parent->type);

    // if (cst->destructor)
    //   cst->destructor(&i->var, vdef);

    FREE(&i->name);
    FREE(&i);
  }
  else
  {
    struct VariableDef *vdef = obj;

    cst = get_type_def(type);
    if (cst->destructor)
      cst->destructor(vdef->var, vdef);
  }
}

struct ConfigSet *cs_set_new(struct ConfigSet *parent, int size)
{
  struct ConfigSet *cs = safe_calloc(1, sizeof(*cs));
  cs_init(cs, parent, size);
  return cs;
}

bool cs_init(struct ConfigSet *cs, struct ConfigSet *parent, int size)
{
  memset(cs, 0, sizeof(*cs));
  cs->hash = hash_create(size, 0);
  hash_set_destructor(cs->hash, destroy, (intptr_t) cs);
  return true;
}

void cs_add_listener(struct ConfigSet *cs, cs_listener fn)
{
  for (unsigned int i = 0; i < mutt_array_size(cs->listeners); i++)
  {
    if (!cs->listeners[i])
    {
      cs->listeners[i] = fn;
      break;
    }
  }
}

void cs_add_validator(struct ConfigSet *cs, cs_validator fn)
{
  cs->validator = fn;
}

void cs_free(struct ConfigSet *cs)
{
  hash_destroy(&cs->hash);
}

void notify_listeners(struct ConfigSet *cs, const char *name, enum ConfigEvent e)
{
  for (unsigned int i = 0; i < mutt_array_size(cs->listeners); i++)
  {
    if (!cs->listeners[i])
      return;

    cs->listeners[i](cs, name, e);
  }
}

void cs_dump_set(struct ConfigSet *cs)
{
  struct HashElem *e = NULL;
  struct HashWalkState state;
  memset(&state, 0, sizeof(state));

  struct Buffer result;
  mutt_buffer_init(&result);
  result.data = calloc(1, STRING);
  result.dsize = STRING;

  while ((e = hash_walk(cs->hash, &state)))
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

    struct VariableDef *vdef = e->data;

    if (type->getter(vdef->var, vdef, &result))
      printf(" = %s\n", result.data);
    else
      printf(": ERROR: %s\n", result.data);
  }

  FREE(&result.data);
}

bool cs_register_type(unsigned int type, const struct ConfigSetType *cst)
{
  if (!cst)
    return false;

  if (!cst->name || !cst->setter || !cst->getter || !cst->resetter)
    return false;

  RegisteredTypes[type] = *cst;
  return false;
}

static struct HashElem *reg_one_var(struct ConfigSet *cs, const struct VariableDef *vdef, struct Buffer *err)
{
  struct ConfigSetType *cst = get_type_def(vdef->type);
  if (!cst)
  {
    mutt_buffer_printf(err, "Variable '%s' has an invalid type %d", vdef->name, vdef->type);
    return NULL;
  }

  struct HashElem *e = hash_typed_insert(cs->hash, vdef->name, vdef->type, (void *) vdef);

  cst->resetter(cs, vdef->var, vdef, err);

  return e;
}

bool cs_register_variables(struct ConfigSet *cs, const struct VariableDef vars[])
{
  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = calloc(1, STRING);
  err.dsize = STRING;

  for (int i = 0; vars[i].name; i++)
  {
    reg_one_var(cs, &vars[i], &err);
    // printf("register: %s\n", vars[i].name);
  }

  FREE(&err.data);
  return true;
}

bool cs_set_variable(struct ConfigSet *cs, const char *name, const char *value, struct Buffer *err)
{
  struct HashElem *e = hash_find_elem(cs->hash, name);
  if (!e)
  {
    mutt_buffer_printf(err, "Unknown var '%s'", name);
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

  void *var = NULL;

  struct VariableDef *vdef = NULL;

  if (e->type & DT_INHERITED)
  {
    struct Inheritance *i = e->data;
    vdef = i->parent->data;
    var = &i->var;
  }
  else
  {
    vdef = e->data;
    var = vdef->var;
  }

  if (!var)
    return false;

  if (!cst->setter(cs, var, vdef, value, err))
    return false;

  if (e->type & DT_INHERITED)
  {
    struct Inheritance *i = e->data;
    e->type = i->parent->type | DT_INHERITED;
  }
  notify_listeners(cs, name, CE_SET);
  return true;
}

bool cs_reset_variable(struct ConfigSet *cs, const char *name, struct Buffer *err)
{
  struct HashElem *e = hash_find_elem(cs->hash, name);
  if (!e)
  {
    mutt_buffer_printf(err, "Unknown var '%s'", name);
    return false;
  }

  /* An inherited var that's already pointing to its parent.
   * Return 'success', but don't send a notification. */
  if ((e->type & DT_INHERITED) && (DTYPE(e->type) == 0))
    return true;

  struct ConfigSetType *cst = NULL;

  if (e->type & DT_INHERITED)
  {
    struct Inheritance *i = e->data;
    cst = get_type_def(i->parent->type);

    struct VariableDef *vdef = i->parent->data;

    if (cst->destructor)
      cst->destructor((void**) &i->var, vdef);

    e->type = DT_INHERITED;
  }
  else
  {
    cst = get_type_def(e->type);

    struct VariableDef *vdef = e->data;

    cst->resetter(cs, vdef->var, vdef, err);
  }

  notify_listeners(cs, name, CE_RESET);
  return true;
}

bool cs_get_variable(struct ConfigSet *cs, const char *name, struct Buffer *result)
{
  if (!cs || !name)
    return false;

  struct HashElem *e = hash_find_elem(cs->hash, name);
  if (!e)
  {
    mutt_buffer_printf(result, "Unknown var '%s'", name);
    return false;
  }

  if ((e->type & DT_INHERITED) && (DTYPE(e->type) == 0))
  {
    // Delegate to parent
    struct Inheritance *i = e->data;
    e = i->parent;
  }

  struct ConfigSetType *cst = get_type_def(e->type);
  if (!cst)
  {
    mutt_buffer_printf(result, "Variable '%s' has an invalid type %d", name, DTYPE(e->type));
    return false;
  }

  void *var = NULL;
  struct VariableDef *vdef = NULL;

  if ((e->type & DT_INHERITED) && (DTYPE(e->type) != 0))
  {
    // Local value
    struct Inheritance *i = e->data;
    vdef = i->parent->data;
    var = &i->var;
  }
  else
  {
    // Normal var
    vdef = e->data;
    if (!vdef)
      return false;

    var = vdef->var;
  }

  if (!cst->getter(var, vdef, result))
    return false;

  return true;
}

struct HashElem *cs_get_elem(struct ConfigSet *cs, const char *name)
{
  if (!cs || !name)
    return NULL;

  struct HashElem *he = hash_find_elem(cs->hash, name);

  if (he && (he->type & DT_INHERITED))
  {
    struct Inheritance *i = he->data;
    he = i->parent;
  }

  return he;
}

struct HashElem *cs_inherit_variable(struct ConfigSet *cs, struct HashElem *parent, const char *name)
{
  if (!cs || !parent)
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

  struct HashElem *e = hash_typed_insert(cs->hash, i->name, DT_INHERITED, i);

  FREE(&err.data);
  return e;
}

