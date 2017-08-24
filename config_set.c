#include "config_set.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inheritance.h"
#include "lib/buffer.h"
#include "lib/hash.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "mutt_options.h"

struct ConfigSetType RegisteredTypes[16] =
{
  { NULL, NULL, NULL, },
};

static void destroy(int type, void *obj, intptr_t data)
{
  struct ConfigSet *cs = (struct ConfigSet *) data;

  const struct ConfigSetType *cst = NULL;

  if (type & DT_INHERITED)
  {
    struct Inheritance *i = obj;
    // struct VariableDef *vdef = i->parent->data;
    // cst = cs_get_type_def(cs, i->parent->type);

    // if (cst->destructor)
    //   cst->destructor(&i->var, vdef);

    FREE(&i->name);
    FREE(&i);
  }
  else
  {
    struct VariableDef *vdef = obj;

    cst = cs_get_type_def(cs, type);
    if (cst && cst->destructor)
      cst->destructor(vdef->var, vdef);
  }
}

static struct HashElem *reg_one_var(const struct ConfigSet *cs, const struct VariableDef *vdef, struct Buffer *err)
{
  const struct ConfigSetType *cst = cs_get_type_def(cs, vdef->type);
  if (!cst)
  {
    mutt_buffer_printf(err, "Variable '%s' has an invalid type %d", vdef->name, vdef->type);
    return NULL;
  }

  struct HashElem *he = hash_typed_insert(cs->hash, vdef->name, vdef->type, (void *) vdef);

  cst->resetter(cs, vdef->var, vdef, err);

  return he;
}

const struct ConfigSetType *cs_get_type_def(const struct ConfigSet *cs, unsigned int type)
{
  type = DTYPE(type);
  if ((type < 1) || (type >= mutt_array_size(cs->types)))
    return NULL;

  return &cs->types[type];
}

void cs_init(struct ConfigSet *cs, int size)
{
  memset(cs, 0, sizeof(*cs));
  cs->hash = hash_create(size, 0);
  hash_set_destructor(cs->hash, destroy, (intptr_t) cs);
}

struct ConfigSet *cs_new_set(int size)
{
  struct ConfigSet *cs = safe_malloc(sizeof(*cs));
  cs_init(cs, size);
  return cs;
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

void cs_free(struct ConfigSet **cs)
{
  if (!cs || !*cs)
    return;

  hash_destroy(&(*cs)->hash);
  FREE(cs);
}

void notify_listeners(const struct ConfigSet *cs, struct HashElem *he, const char *name, enum ConfigEvent ev)
{
  for (unsigned int i = 0; i < mutt_array_size(cs->listeners); i++)
  {
    if (!cs->listeners[i])
      return;

    cs->listeners[i](cs, he, name, ev);
  }
}

bool cs_register_type(struct ConfigSet *cs, unsigned int type, const struct ConfigSetType *cst)
{
  if (!cs || !cst)
    return false;

  if (!cst->name || !cst->setter || !cst->getter || !cst->resetter || !cst->nsetter || !cst->ngetter)
    return false;

  if (type >= mutt_array_size(cs->types))
    return false;

  if (cs->types[type].name)
    return false; // already registered?

  cs->types[type] = *cst;
  return true;
}

bool cs_register_variables(const struct ConfigSet *cs, const struct VariableDef vars[])
{
  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = calloc(1, STRING);
  err.dsize = STRING;

  bool result = true;

  for (int i = 0; vars[i].name; i++)
  {
    if (!reg_one_var(cs, &vars[i], &err))
    {
      // printf("register: %s\n", vars[i].name);
      result = false;
    }
  }

  FREE(&err.data);
  return result;
}

bool cs_set_variable(const struct ConfigSet *cs, const char *name, const char *value, struct Buffer *err)
{
  struct HashElem *he = hash_find_elem(cs->hash, name);
  if (!he)
  {
    mutt_buffer_printf(err, "Unknown var '%s'", name);
    return false;
  }

  const struct ConfigSetType *cst = NULL;

  if (he->type & DT_INHERITED)
  {
    struct Inheritance *i = he->data;
    cst = cs_get_type_def(cs, i->parent->type);
  }
  else
  {
    cst = cs_get_type_def(cs, he->type);
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

bool cs_reset_variable(const struct ConfigSet *cs, const char *name, struct Buffer *err)
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

  const struct ConfigSetType *cst = NULL;

  if (he->type & DT_INHERITED)
  {
    struct Inheritance *i = he->data;
    cst = cs_get_type_def(cs, i->parent->type);

    struct VariableDef *vdef = i->parent->data;

    if (cst && cst->destructor)
      cst->destructor((void**) &i->var, vdef);

    he->type = DT_INHERITED;
  }
  else
  {
    cst = cs_get_type_def(cs, he->type);

    const struct VariableDef *vdef = he->data;

    if (cst)
      cst->resetter(cs, vdef->var, vdef, err);
  }

  notify_listeners(cs, he, name, CE_RESET);
  return true;
}

bool cs_get_variable(const struct ConfigSet *cs, const char *name, struct Buffer *result)
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
  const struct VariableDef *vdef = NULL;
  const struct ConfigSetType *cst = NULL;
  void *var = NULL;

  if (he->type & DT_INHERITED)
  {
    i = he->data;
    vdef = i->parent->data;
    cst = cs_get_type_def(cs, i->parent->type);
  }
  else
  {
    vdef = he->data;
    cst = cs_get_type_def(cs, he->type);
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

bool cs_get_variable2(const struct ConfigSet *cs, const struct HashElem *he, struct Buffer *result)
{
  if (!cs || !he)
    return false;

  struct Inheritance *i = NULL;
  const struct VariableDef *vdef = NULL;
  const struct ConfigSetType *cst = NULL;
  void *var = NULL;

  if (he->type & DT_INHERITED)
  {
    i = he->data;
    vdef = i->parent->data;
    cst = cs_get_type_def(cs, i->parent->type);
  }
  else
  {
    vdef = he->data;
    cst = cs_get_type_def(cs, he->type);
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
    mutt_buffer_printf(result, "Variable '%s' has an invalid type %d", vdef->name, DTYPE(he->type));
    return false;
  }

  return cst->getter(var, vdef, result);
}

struct HashElem *cs_get_elem(const struct ConfigSet *cs, const char *name)
{
  if (!cs || !name)
    return NULL;

  return hash_find_elem(cs->hash, name);
}

struct HashElem *cs_inherit_variable(const struct ConfigSet *cs, struct HashElem *parent, const char *name)
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


bool cs_he_set_value(const struct ConfigSet *cs, struct HashElem *he, intptr_t value, struct Buffer *err)
{
  if (!cs || !he)
    return false;

  const struct VariableDef *vdef = NULL;
  const struct ConfigSetType *cst = NULL;
  void *var = NULL;

  if (he->type & DT_INHERITED)
  {
    struct Inheritance *i = he->data;
    vdef = i->parent->data;
    var = &i->var;
    cst = cs_get_type_def(cs, i->parent->type);
  }
  else
  {
    vdef = he->data;
    var = vdef->var;
    cst = cs_get_type_def(cs, he->type);
  }

  if (!cst)
  {
    mutt_buffer_printf(err, "Variable '%s' has an invalid type %d", vdef->name, he->type);
    return false;
  }

  bool result = cst->nsetter(cs, var, vdef, value, err);
  if (result)
  {
    if (he->type & DT_INHERITED)
      he->type = DT_INHERITED | vdef->type;
    notify_listeners(cs, he, vdef->name, CE_SET);
  }

  return result;
}

bool cs_he_get_value(const struct ConfigSet *cs, struct HashElem *he, struct Buffer *err)
{
  if (!cs || !he)
    return false;

  const struct VariableDef *vdef = NULL;
  const struct ConfigSetType *cst = NULL;
  void *var = NULL;

  if (he->type & DT_INHERITED)
  {
    struct Inheritance *i = he->data;
    vdef = i->parent->data;
    var = &i->var;
    cst = cs_get_type_def(cs, i->parent->type);
  }
  else
  {
    vdef = he->data;
    var = vdef->var;
    cst = cs_get_type_def(cs, he->type);
  }

  if (!cst)
  {
    mutt_buffer_printf(err, "Variable '%s' has an invalid type %d", vdef->name, he->type);
    return false;
  }

  return cst->ngetter(cs, var, vdef, err);
}


bool cs_str_set_value(const struct ConfigSet *cs, const char *name, intptr_t value, struct Buffer *err)
{
  if (!cs || !name)
    return false;

  struct HashElem *he = cs_get_elem(cs, name);
  if (!he)
  {
    mutt_buffer_printf(err, "Unknown var '%s'", name);
    return false;
  }

  const struct VariableDef *vdef = NULL;
  const struct ConfigSetType *cst = NULL;
  void *var = NULL;

  if (he->type & DT_INHERITED)
  {
    struct Inheritance *i = he->data;
    vdef = i->parent->data;
    var = &i->var;
    cst = cs_get_type_def(cs, i->parent->type);
  }
  else
  {
    vdef = he->data;
    var = vdef->var;
    cst = cs_get_type_def(cs, he->type);
  }

  if (!cst)
  {
    mutt_buffer_printf(err, "Variable '%s' has an invalid type %d", vdef->name, he->type);
    return false;
  }

  bool result = cst->nsetter(cs, var, vdef, value, err);
  if (result)
  {
    if (he->type & DT_INHERITED)
      he->type = DT_INHERITED | vdef->type;
    notify_listeners(cs, he, vdef->name, CE_SET);
  }

  return result;
}

intptr_t cs_str_get_value(const struct ConfigSet *cs, const char *name, struct Buffer *err)
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
  const struct VariableDef *vdef = NULL;
  const struct ConfigSetType *cst = NULL;
  void *var = NULL;

  if (he->type & DT_INHERITED)
  {
    i = he->data;
    vdef = i->parent->data;
    cst = cs_get_type_def(cs, i->parent->type);
  }
  else
  {
    vdef = he->data;
    cst = cs_get_type_def(cs, he->type);
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

