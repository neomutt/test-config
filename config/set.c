#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "set.h"
#include "inheritance.h"
#include "lib/buffer.h"
#include "lib/hash.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "types.h"

struct ConfigSetType RegisteredTypes[16] = {
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL },
};

static void destroy(int type, void *obj, intptr_t data)
{
  if (!obj)
    return; /* LCOV_EXCL_LINE */

  struct ConfigSet *cs = (struct ConfigSet *) data;
  if (!cs)
    return;

  const struct ConfigSetType *cst = NULL;

  if (type & DT_INHERITED)
  {
    struct Inheritance *i = obj;
    // struct VariableDef *vdef = i->parent->data;
    // cst = cs_get_type_def(cs, i->parent->type);

    // if (cst->destroy)
    //   cst->destroy(&i->var, vdef);

    FREE(&i->name);
    FREE(&i);
  }
  else
  {
    struct VariableDef *vdef = obj;

    cst = cs_get_type_def(cs, type);
    if (cst && cst->destroy)
      cst->destroy(vdef->var, vdef);

    /* If we allocated the initial value, clean it up */
    if (type & DT_INITIAL_SET)
      FREE(&vdef->initial);
  }
}

static struct HashElem *create_synonym(const struct ConfigSet *cs,
                                       struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !vdef)
    return NULL; /* LCOV_EXCL_LINE */

  const char *name = (const char *) vdef->initial;
  struct HashElem *parent = cs_get_elem(cs, name);
  if (!parent)
  {
    mutt_buffer_printf(err, "No such variable: %s", name);
    return NULL;
  }

  struct HashElem *child =
      hash_typed_insert(cs->hash, vdef->name, vdef->type, (void *) vdef);
  if (!child)
    return NULL;

  vdef->var = parent;
  return child;
}

static struct HashElem *reg_one_var(const struct ConfigSet *cs,
                                    struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !vdef)
    return NULL; /* LCOV_EXCL_LINE */

  if (vdef->type == DT_SYNONYM)
    return create_synonym(cs, vdef, err);

  const struct ConfigSetType *cst = cs_get_type_def(cs, vdef->type);
  if (!cst)
  {
    mutt_buffer_printf(err, "Variable '%s' has an invalid type %d", vdef->name, vdef->type);
    return NULL;
  }

  struct HashElem *he =
      hash_typed_insert(cs->hash, vdef->name, vdef->type, (void *) vdef);
  if (!he)
    return NULL;

  if (cst && cst->reset)
    cst->reset(cs, vdef->var, vdef, err);

  return he;
}

const struct ConfigSetType *cs_get_type_def(const struct ConfigSet *cs, unsigned int type)
{
  if (!cs)
    return NULL; /* LCOV_EXCL_LINE */

  type = DTYPE(type);
  if ((type < 1) || (type >= mutt_array_size(cs->types)))
    return NULL;

  if (!cs->types[type].name)
    return NULL;

  return &cs->types[type];
}

void cs_init(struct ConfigSet *cs, int size)
{
  if (!cs)
    return; /* LCOV_EXCL_LINE */

  memset(cs, 0, sizeof(*cs));
  cs->hash = hash_create(size, 0);
  hash_set_destructor(cs->hash, destroy, (intptr_t) cs);
}

struct ConfigSet *cs_create(int size)
{
  struct ConfigSet *cs = safe_malloc(sizeof(*cs));
  cs_init(cs, size);
  return cs;
}

void cs_add_listener(struct ConfigSet *cs, cs_listener fn)
{
  if (!cs || !fn)
    return;

  for (unsigned int i = 0; i < mutt_array_size(cs->listeners); i++)
  {
    if (!cs->listeners[i])
    {
      cs->listeners[i] = fn;
      break;
    }
  }
}

void cs_remove_listener(struct ConfigSet *cs, cs_listener fn)
{
  if (!cs || !fn)
    return;

  for (unsigned int i = 0; i < mutt_array_size(cs->listeners); i++)
  {
    if (cs->listeners[i] == fn)
    {
      cs->listeners[i] = NULL;
      break;
    }
  }
}

void cs_free(struct ConfigSet **cs)
{
  if (!cs || !*cs)
    return; /* LCOV_EXCL_LINE */

  hash_destroy(&(*cs)->hash);
  FREE(cs);
}

void notify_listeners(const struct ConfigSet *cs, struct HashElem *he,
                      const char *name, enum ConfigEvent ev)
{
  if (!cs || !he || !name)
    return; /* LCOV_EXCL_LINE */

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
    return false; /* LCOV_EXCL_LINE */

  if (!cst->name || !cst->string_set || !cst->string_get || !cst->reset ||
      !cst->native_set || !cst->native_get)
    return false;

  if (type >= mutt_array_size(cs->types))
    return false;

  if (cs->types[type].name)
    return false; // already registered?

  cs->types[type] = *cst;
  return true;
}

bool cs_register_variables(const struct ConfigSet *cs, struct VariableDef vars[])
{
  if (!cs || !vars)
    return false; /* LCOV_EXCL_LINE */

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = calloc(1, STRING);
  err.dsize = STRING;

  bool result = true;

  for (int i = 0; vars[i].name; i++)
  {
    if (!reg_one_var(cs, &vars[i], &err))
    {
      printf("%s\n", err.data);
      result = false;
    }
  }

  FREE(&err.data);
  return result;
}

bool cs_set_variable(const struct ConfigSet *cs, const char *name,
                     const char *value, struct Buffer *err)
{
  if (!cs || !name)
    return false; /* LCOV_EXCL_LINE */

  struct HashElem *he = cs_get_elem(cs, name);
  if (!he)
  {
    mutt_buffer_printf(err, "Unknown var '%s'", name);
    return false;
  }

  const struct ConfigSetType *cst = NULL;
  const char *notify_name = NULL;

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
    notify_name = name;
  }
  else
  {
    vdef = he->data;
    var = vdef->var;
    notify_name = vdef->name;
  }

  if (!var)
    return false;

  if (!cst->string_set(cs, var, vdef, value, err))
    return false;

  if (he->type & DT_INHERITED)
  {
    struct Inheritance *i = he->data;
    he->type = i->parent->type | DT_INHERITED;
  }
  notify_listeners(cs, he, notify_name, CE_SET);
  return true;
}

bool cs_reset_variable(const struct ConfigSet *cs, const char *name, struct Buffer *err)
{
  if (!cs || !name)
    return false; /* LCOV_EXCL_LINE */

  struct HashElem *he = cs_get_elem(cs, name);
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
  const struct VariableDef *vdef = NULL;
  const char *notify_name = NULL;

  if (he->type & DT_INHERITED)
  {
    struct Inheritance *i = he->data;
    cst = cs_get_type_def(cs, i->parent->type);
    vdef = i->parent->data;
    notify_name = name;

    if (cst && cst->destroy)
      cst->destroy((void **) &i->var, vdef);

    he->type = DT_INHERITED;
  }
  else
  {
    cst = cs_get_type_def(cs, he->type);
    vdef = he->data;
    notify_name = vdef->name;

    if (cst)
      cst->reset(cs, vdef->var, vdef, err);
  }

  notify_listeners(cs, he, notify_name, CE_RESET);
  return true;
}

bool cs_get_variable(const struct ConfigSet *cs, const char *name, struct Buffer *result)
{
  if (!cs || !name)
    return false; /* LCOV_EXCL_LINE */

  struct HashElem *he = cs_get_elem(cs, name);
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
    var = &i->var; // Local value
  }
  else
  {
    var = vdef->var; // Normal var
  }

  if (!cst)
  {
    mutt_buffer_printf(result, "Variable '%s' has an invalid type %d", name,
                       DTYPE(he->type));
    return false;
  }

  return cst->string_get(var, vdef, result);
}

struct HashElem *cs_get_elem(const struct ConfigSet *cs, const char *name)
{
  if (!cs || !name)
    return NULL; /* LCOV_EXCL_LINE */

  struct HashElem *he = hash_find_elem(cs->hash, name);
  if (!he)
    return NULL;

  if (he->type != DT_SYNONYM)
    return he;

  const struct VariableDef *vdef = he->data;

  return vdef->var;
}

struct HashElem *cs_inherit_variable(const struct ConfigSet *cs,
                                     struct HashElem *parent, const char *name)
{
  if (!cs || !parent)
    return NULL; /* LCOV_EXCL_LINE */

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


bool cs_he_set_value(const struct ConfigSet *cs, struct HashElem *he,
                     intptr_t value, struct Buffer *err)
{
  if (!cs || !he)
    return false; /* LCOV_EXCL_LINE */

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

  bool result = cst->native_set(cs, var, vdef, value, err);
  if (result)
  {
    if (he->type & DT_INHERITED)
      he->type = DT_INHERITED | vdef->type;
    notify_listeners(cs, he, vdef->name, CE_SET);
  }

  return result;
}

bool cs_he_get_value(const struct ConfigSet *cs, struct HashElem *he, struct Buffer *result)
{
  if (!cs || !he)
    return false; /* LCOV_EXCL_LINE */

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
    var = &i->var; // Local value
  }
  else
  {
    var = vdef->var; // Normal var
  }

  if (!cst)
  {
    mutt_buffer_printf(result, "Variable '%s' has an invalid type %d",
                       vdef->name, DTYPE(he->type));
    return false;
  }

  return cst->string_get(var, vdef, result);
}


bool cs_str_set_value(const struct ConfigSet *cs, const char *name,
                      intptr_t value, struct Buffer *err)
{
  if (!cs || !name)
    return false; /* LCOV_EXCL_LINE */

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

  bool result = cst->native_set(cs, var, vdef, value, err);
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
    return -1; /* LCOV_EXCL_LINE */

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

  return cst->native_get(cs, var, vdef, err);
}

bool cs_set_initial_value(const struct ConfigSet *cs, struct HashElem *he,
                          const char *value, struct Buffer *err)
{
  if (!cs || !he)
    return false; /* LCOV_EXCL_LINE */

  if (he->type & DT_INHERITED)
    return false;

  /* We've been here before */
  if (he->type & DT_INITIAL_SET)
    return false;

  struct VariableDef *vdef = he->data;
  if (!vdef)
    return false;

  if (vdef->initial != 0)
    return false;

  vdef->initial = IP safe_strdup(value);
  he->type |= DT_INITIAL_SET;

  return cs_reset_variable(cs, vdef->name, err);
}
