#include "config_set.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/lib.h"
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
    if (cst && cst->destructor)
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

void notify_listeners(struct ConfigSet *cs, struct HashElem *he, const char *name, enum ConfigEvent ev)
{
  for (unsigned int i = 0; i < mutt_array_size(cs->listeners); i++)
  {
    if (!cs->listeners[i])
      return;

    cs->listeners[i](cs, he, name, ev);
  }
}

void cs_dump_set(struct ConfigSet *cs)
{
  struct HashElem *he = NULL;
  struct HashWalkState state;
  memset(&state, 0, sizeof(state));

  struct Buffer result;
  mutt_buffer_init(&result);
  result.data = calloc(1, STRING);
  result.dsize = STRING;

  while ((he = hash_walk(cs->hash, &state)))
  {
    const char *name = NULL;

    if (he->type & DT_INHERITED)
    {
      struct Inheritance *i = he->data;
      he = i->parent;
      name = i->name;
    }
    else
    {
      name = he->key.strkey;
    }

    struct ConfigSetType *cst = get_type_def(he->type);
    if (!cst)
    {
      printf("Unknown type: %d\n", he->type);
      continue;
    }

    mutt_buffer_reset(&result);
    printf("%-7s %s", cst->name, name);

    struct VariableDef *vdef = he->data;

    if (cst->getter(vdef->var, vdef, &result))
      printf(" = %s\n", result.data);
    else
      printf(": ERROR: %s\n", result.data);
  }

  FREE(&result.data);
}

bool cs_register_type(unsigned int type, struct ConfigSetType *cst)
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

  struct HashElem *he = hash_typed_insert(cs->hash, vdef->name, vdef->type, (void *) vdef);

  cst->resetter(cs, vdef->var, vdef, err);

  return he;
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
  struct HashElem *he = hash_find_elem(cs->hash, name);
  if (!he)
  {
    mutt_buffer_printf(err, "Unknown var '%s'", name);
    return false;
  }

  struct ConfigSetType *cst = NULL;

  if (he->type & DT_INHERITED)
  {
    struct Inheritance *i = he->data;
    cst = get_type_def(i->parent->type);
  }
  else
  {
    cst = get_type_def(he->type);
  }

  if (!cst)
  {
    mutt_buffer_printf(err, "Variable '%s' has an invalid type %d", name, he->type);
    return false;
  }

  void *var = NULL;

  struct VariableDef *vdef = NULL;

  if (he->type & DT_INHERITED)
  {
    struct Inheritance *i = he->data;
    vdef = i->parent->data;
    var = &i->var;
  }
  else
  {
    vdef = he->data;
    var = vdef->var;
  }

  if (!var)
    return false;

  if (!cst->setter(cs, var, vdef, value, err))
    return false;

  if (he->type & DT_INHERITED)
  {
    struct Inheritance *i = he->data;
    he->type = i->parent->type | DT_INHERITED;
  }
  notify_listeners(cs, he, name, CE_SET);
  return true;
}

bool cs_reset_variable(struct ConfigSet *cs, const char *name, struct Buffer *err)
{
  struct HashElem *he = hash_find_elem(cs->hash, name);
  if (!he)
  {
    mutt_buffer_printf(err, "Unknown var '%s'", name);
    return false;
  }

  /* An inherited var that's already pointing to its parent.
   * Return 'success', but don't send a notification. */
  if ((he->type & DT_INHERITED) && (DTYPE(he->type) == 0))
    return true;

  struct ConfigSetType *cst = NULL;

  if (he->type & DT_INHERITED)
  {
    struct Inheritance *i = he->data;
    cst = get_type_def(i->parent->type);

    struct VariableDef *vdef = i->parent->data;

    if (cst && cst->destructor)
      cst->destructor((void**) &i->var, vdef);

    he->type = DT_INHERITED;
  }
  else
  {
    cst = get_type_def(he->type);

    struct VariableDef *vdef = he->data;

    if (cst)
      cst->resetter(cs, vdef->var, vdef, err);
  }

  notify_listeners(cs, he, name, CE_RESET);
  return true;
}

bool cs_get_variable(struct ConfigSet *cs, const char *name, struct Buffer *result)
{
  if (!cs || !name)
    return false;

  struct HashElem *he = hash_find_elem(cs->hash, name);
  if (!he)
  {
    mutt_buffer_printf(result, "Unknown var '%s'", name);
    return false;
  }

  struct Inheritance *i = NULL;
  struct VariableDef *vdef = NULL;
  struct ConfigSetType *cst = NULL;
  void *var = NULL;

  if (he->type & DT_INHERITED)
  {
    i = he->data;
    vdef = i->parent->data;
    cst = get_type_def(i->parent->type);
  }
  else
  {
    vdef = he->data;
    cst = get_type_def(he->type);
  }

  if ((he->type & DT_INHERITED) && (DTYPE(he->type) != 0))
  {
    var = &i->var;  // Local value
  }
  else
  {
    var = vdef->var; // Normal var
  }

  if (!cst)
  {
    mutt_buffer_printf(result, "Variable '%s' has an invalid type %d", name, DTYPE(he->type));
    return false;
  }

  return cst->getter(var, vdef, result);
}

struct HashElem *cs_get_elem(struct ConfigSet *cs, const char *name)
{
  if (!cs || !name)
    return NULL;

  return hash_find_elem(cs->hash, name);
}

struct HashElem *cs_inherit_variable(struct ConfigSet *cs, struct HashElem *parent, const char *name)
{
  if (!cs || !parent)
    return NULL;

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = calloc(1, STRING);
  err.dsize = STRING;

  struct Inheritance *i = safe_calloc(1, sizeof(*i));
  i->parent = parent;
  i->name = safe_strdup(name);

  struct HashElem *he = hash_typed_insert(cs->hash, i->name, DT_INHERITED, i);

  FREE(&err.data);
  return he;
}


bool cs_set_value(struct ConfigSet *cs, struct HashElem *he, intptr_t value, struct Buffer *err)
{
  if (!cs || !he)
    return false;

  struct VariableDef *vdef = NULL;
  struct ConfigSetType *cst = NULL;
  void *var = NULL;

  if (he->type & DT_INHERITED)
  {
    struct Inheritance *i = he->data;
    vdef = i->parent->data;
    var = &i->var;
    cst = get_type_def(i->parent->type);
  }
  else
  {
    vdef = he->data;
    var = vdef->var;
    cst = get_type_def(he->type);
  }

  if (!cst)
  {
    mutt_buffer_printf(err, "Variable '%s' has an invalid type %d", vdef->name, he->type);
    return false;
  }

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  if (cst->destructor)
    cst->destructor(var, vdef);

  if (!cst->nsetter) //XXX compulsory
    return false;

  bool result = cst->nsetter(cs, var, vdef, value, err);
  if (result)
  {
    if (he->type & DT_INHERITED)
      he->type = DT_INHERITED | vdef->type;
    notify_listeners(cs, he, vdef->name, CE_SET);
  }

  return result;
}

bool cs_get_value(struct ConfigSet *cs, struct HashElem *he, struct Buffer *err)
{
  if (!cs || !he)
    return false;

  struct VariableDef *vdef = NULL;
  struct ConfigSetType *cst = NULL;
  void *var = NULL;

  if (he->type & DT_INHERITED)
  {
    struct Inheritance *i = he->data;
    vdef = i->parent->data;
    var = &i->var;
    cst = get_type_def(i->parent->type);
  }
  else
  {
    vdef = he->data;
    var = vdef->var;
    cst = get_type_def(he->type);
  }

  if (!cst)
  {
    mutt_buffer_printf(err, "Variable '%s' has an invalid type %d", vdef->name, he->type);
    return false;
  }

  return cst->ngetter(cs, var, vdef, err);
}


bool cs_set_value2(struct ConfigSet *cs, const char *name, intptr_t value, struct Buffer *err)
{
  if (!cs || !name)
    return false;

  struct HashElem *he = cs_get_elem(cs, name);
  if (!he)
  {
    mutt_buffer_printf(err, "Unknown var '%s'", name);
    return false;
  }

  struct VariableDef *vdef = NULL;
  struct ConfigSetType *cst = NULL;
  void *var = NULL;

  if (he->type & DT_INHERITED)
  {
    struct Inheritance *i = he->data;
    vdef = i->parent->data;
    var = &i->var;
    cst = get_type_def(i->parent->type);
  }
  else
  {
    vdef = he->data;
    var = vdef->var;
    cst = get_type_def(he->type);
  }

  if (!cst)
  {
    mutt_buffer_printf(err, "Variable '%s' has an invalid type %d", vdef->name, he->type);
    return false;
  }

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  if (cst->destructor)
    cst->destructor(var, vdef);

  if (!cst->nsetter) //XXX compulsory
    return false;

  bool result = cst->nsetter(cs, var, vdef, value, err);
  if (result)
  {
    if (he->type & DT_INHERITED)
      he->type = DT_INHERITED | vdef->type;
    notify_listeners(cs, he, vdef->name, CE_SET);
  }

  return result;
}

intptr_t cs_get_value2(struct ConfigSet *cs, const char *name, struct Buffer *err)
{
  if (!cs || !name)
    return -1;

  struct HashElem *he = cs_get_elem(cs, name);
  if (!he)
  {
    mutt_buffer_printf(err, "Unknown var '%s'", name);
    return -1;
  }

  struct Inheritance *i = NULL;
  struct VariableDef *vdef = NULL;
  struct ConfigSetType *cst = NULL;
  void *var = NULL;

  if (he->type & DT_INHERITED)
  {
    i = he->data;
    vdef = i->parent->data;
    cst = get_type_def(i->parent->type);
  }
  else
  {
    vdef = he->data;
    cst = get_type_def(he->type);
  }

  if ((he->type & DT_INHERITED) && (DTYPE(he->type) != 0))
  {
    var = &i->var;
  }
  else
  {
    var = vdef->var;
  }

  if (!cst)
  {
    mutt_buffer_printf(err, "Variable '%s' has an invalid type %d", vdef->name, he->type);
    return -1;
  }

  return cst->ngetter(cs, var, vdef, err);
}

