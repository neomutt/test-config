/**
 * @file
 * A collection of config items
 *
 * @authors
 * Copyright (C) 2017 Richard Russon <rich@flatcap.org>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "set.h"
#include "inheritance.h"
#include "lib/buffer.h"
#include "lib/debug.h"
#include "lib/hash.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "types.h"

struct ConfigSetType RegisteredTypes[16] = {
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL },
};

/**
 * destroy - Callback function for the Hash Table
 * @param type Object type, e.g. #DT_STRING
 * @param obj  Object to destroy
 * @param data ConfigSet associated with the object
 */
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
    //   cst->destroy(cs, &i->var, vdef);

    FREE(&i->name);
    FREE(&i);
  }
  else
  {
    struct VariableDef *vdef = obj;

    cst = cs_get_type_def(cs, type);
    if (cst && cst->destroy)
      cst->destroy(cs, vdef->var, vdef);

    /* If we allocated the initial value, clean it up */
    if (type & DT_INITIAL_SET)
      FREE(&vdef->initial);
  }
}

/**
 * create_synonym - Create an alternative name for a config item
 * @param cs   Config items
 * @param vdef Variable definition
 * @param err  Buffer for error messages
 * @retval ptr New HashElem representing the config item synonym
 */
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

/**
 * reg_one_var - Register one config item
 * @param cs   Config items
 * @param vdef Variable definition
 * @param err  Buffer for error messages
 * @retval ptr New HashElem representing the config item
 */
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

/**
 * cs_get_type_def - Get the definition for a type
 * @param cs   Config items
 * @param type Type to lookup, e.g. #DT_NUMBER
 * @retval ptr ConfigSetType representing the type
 */
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

/**
 * cs_init - Initialise a Config Set
 * @param cs   Config items
 * @param size Number of expected config items
 */
void cs_init(struct ConfigSet *cs, int size)
{
  if (!cs)
    return; /* LCOV_EXCL_LINE */

  memset(cs, 0, sizeof(*cs));
  cs->hash = hash_create(size, 0);
  hash_set_destructor(cs->hash, destroy, (intptr_t) cs);
}

/**
 * cs_create - Create a new Config Set
 * @param size Number of expected config items
 * @retval ptr New ConfigSet object
 */
struct ConfigSet *cs_create(int size)
{
  struct ConfigSet *cs = safe_malloc(sizeof(*cs));
  cs_init(cs, size);
  return cs;
}

/**
 * cs_add_listener - Add a listener (callback function)
 * @param cs Config items
 * @param fn Listener callback function 
 */
void cs_add_listener(struct ConfigSet *cs, cs_listener fn)
{
  if (!cs || !fn)
    return; /* LCOV_EXCL_LINE */

  // check for dupes
  for (unsigned int i = 0; i < mutt_array_size(cs->listeners); i++)
  {
    if (cs->listeners[i] == fn)
    {
      mutt_debug(1, "Listener was already registered\n");
      return;
    }
  }

  for (unsigned int i = 0; i < mutt_array_size(cs->listeners); i++)
  {
    if (!cs->listeners[i])
    {
      cs->listeners[i] = fn;
      return;
    }
  }
}

/**
 * cs_remove_listener - Remove a listener (callback function)
 * @param cs Config items
 * @param fn Listener callback function 
 */
void cs_remove_listener(struct ConfigSet *cs, cs_listener fn)
{
  if (!cs || !fn)
    return; /* LCOV_EXCL_LINE */

  for (unsigned int i = 0; i < mutt_array_size(cs->listeners); i++)
  {
    if (cs->listeners[i] == fn)
    {
      cs->listeners[i] = NULL;
      return;
    }
  }
  mutt_debug(1, "Listener wasn't registered\n");
}

/**
 * cs_free - Free a Config Set
 * @param cs Config items
 */
void cs_free(struct ConfigSet **cs)
{
  if (!cs || !*cs)
    return; /* LCOV_EXCL_LINE */

  hash_destroy(&(*cs)->hash);
  FREE(cs);
}

/**
 * cs_notify_listeners - Notify all listeners of an event
 * @param cs   Config items
 * @param he   HashElem representing config item
 * @param name Name of config item 
 * @param ev   Type of event
 */
void cs_notify_listeners(const struct ConfigSet *cs, struct HashElem *he,
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

/**
 * cs_register_type - Register a type of config item
 * @param cs   Config items
 * @param type Object type, e.g. #DT_BOOL
 * @param cst  Structure defining the type
 * @retval bool True, if type was registered successfully
 */
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

/**
 * cs_register_variables - Register a set of config items
 * @param cs   Config items
 * @param vars Variable definition
 * @retval bool True, if all variables were registered successfully
 */
bool cs_register_variables(const struct ConfigSet *cs, struct VariableDef vars[])
{
  if (!cs || !vars)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = calloc(1, STRING);
  err.dsize = STRING;

  bool result = true;

  for (int i = 0; vars[i].name; i++)
  {
    if (!reg_one_var(cs, &vars[i], &err))
    {
      mutt_debug(1, "%s\n", err.data);
      result = false;
    }
  }

  FREE(&err.data);
  return result;
}

/**
 * cs_str_string_set - Set a config item by string
 * @param cs    Config items
 * @param name  Name of config item
 * @param value Value to set
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
int cs_str_string_set(const struct ConfigSet *cs, const char *name,
                      const char *value, struct Buffer *err)
{
  if (!cs || !name)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct HashElem *he = cs_get_elem(cs, name);
  if (!he)
  {
    mutt_buffer_printf(err, "Unknown var '%s'", name);
    return CSR_ERR_UNKNOWN;
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
    mutt_debug(1, "Variable '%s' has an invalid type %d\n", name, he->type);
    return CSR_ERR_CODE;
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
    return CSR_ERR_CODE;

  int result = cst->string_set(cs, var, vdef, value, err);
  if (CSR_RESULT(result) != CSR_SUCCESS)
    return result;

  if (he->type & DT_INHERITED)
  {
    struct Inheritance *i = he->data;
    he->type = i->parent->type | DT_INHERITED;
  }
  cs_notify_listeners(cs, he, notify_name, CE_SET);
  return CSR_SUCCESS;
}

/**
 * cs_reset_variable - Reset a config item to its initial value
 * @param cs   Config items
 * @param name Name of config item
 * @param err  Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
int cs_reset_variable(const struct ConfigSet *cs, const char *name, struct Buffer *err)
{
  if (!cs || !name)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct HashElem *he = cs_get_elem(cs, name);
  if (!he)
  {
    mutt_buffer_printf(err, "Unknown var '%s'", name);
    return CSR_ERR_UNKNOWN;
  }

  /* An inherited var that's already pointing to its parent.
   * Return 'success', but don't send a notification. */
  if ((he->type & DT_INHERITED) && (DTYPE(he->type) == 0))
    return CSR_SUCCESS;

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
      cst->destroy(cs, (void **) &i->var, vdef);

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

  cs_notify_listeners(cs, he, notify_name, CE_RESET);
  return CSR_SUCCESS;
}

/**
 * cs_str_string_get - Get a config item as a string
 * @param cs     Config items
 * @param name   Name of config item
 * @param result Buffer for results or error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
int cs_str_string_get(const struct ConfigSet *cs, const char *name, struct Buffer *result)
{
  if (!cs || !name)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct HashElem *he = cs_get_elem(cs, name);
  if (!he)
  {
    mutt_buffer_printf(result, "Unknown var '%s'", name);
    return CSR_ERR_UNKNOWN;
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
    mutt_debug(1, "Variable '%s' has an invalid type %d\n", name, DTYPE(he->type));
    return CSR_ERR_CODE;
  }

  return cst->string_get(cs, var, vdef, result);
}

/**
 * cs_get_elem - Get the HashElem representing a config item
 * @param cs   Config items
 * @param name Name of config item
 * @retval ptr HashElem representing the config item
 */
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

/**
 * cs_inherit_variable - Create in inherited config item
 * @param cs     Config items
 * @param parent HashElem of parent config item
 * @param name   Name of account-specific config item
 * @retval ptr New HashElem representing the inherited config item
 */
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
  if (!he)
  {
    FREE(&i->name);
    FREE(&i);
  }

  FREE(&err.data);
  return he;
}


/**
 * cs_he_native_set - Natively set the value of a HashElem config item
 * @param cs    Config items
 * @param he    HashElem representing config item
 * @param value Native pointer/value to set
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
int cs_he_native_set(const struct ConfigSet *cs, struct HashElem *he,
                     intptr_t value, struct Buffer *err)
{
  if (!cs || !he)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

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
    mutt_debug(1, "Variable '%s' has an invalid type %d\n", vdef->name, he->type);
    return CSR_ERR_CODE;
  }

  int result = cst->native_set(cs, var, vdef, value, err);
  if (CSR_RESULT(result) == CSR_SUCCESS)
  {
    if (he->type & DT_INHERITED)
      he->type = DT_INHERITED | vdef->type;
    cs_notify_listeners(cs, he, vdef->name, CE_SET);
  }

  return result;
}

/**
 * cs_he_native_get - Natively get the value of a HashElem config item
 * @param cs     Config items
 * @param he     HashElem representing config item
 * @param result Buffer for results or error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
int cs_he_native_get(const struct ConfigSet *cs, struct HashElem *he, struct Buffer *result)
{
  if (!cs || !he)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

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
    mutt_debug(1, "Variable '%s' has an invalid type %d\n", vdef->name, DTYPE(he->type));
    return CSR_ERR_CODE;
  }

  return cst->string_get(cs, var, vdef, result);
}


/**
 * cs_str_native_set - Natively set the value of a string config item
 * @param cs    Config items
 * @param name  Name of config item
 * @param value Native pointer/value to set
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
int cs_str_native_set(const struct ConfigSet *cs, const char *name,
                      intptr_t value, struct Buffer *err)
{
  if (!cs || !name)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct HashElem *he = cs_get_elem(cs, name);
  if (!he)
  {
    mutt_buffer_printf(err, "Unknown var '%s'", name);
    return CSR_ERR_UNKNOWN;
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
    mutt_debug(1, "Variable '%s' has an invalid type %d\n", vdef->name, he->type);
    return CSR_ERR_CODE;
  }

  int result = cst->native_set(cs, var, vdef, value, err);
  if (CSR_RESULT(result) == CSR_SUCCESS)
  {
    if (he->type & DT_INHERITED)
      he->type = DT_INHERITED | vdef->type;
    cs_notify_listeners(cs, he, vdef->name, CE_SET);
  }

  return result;
}

/**
 * cs_str_native_get - Natively get the value of a string config item
 * @param cs   Config items
 * @param name Name of config item
 * @param err  Buffer for error messages
 * @retval intptr_t Native pointer/value
 */
intptr_t cs_str_native_get(const struct ConfigSet *cs, const char *name, struct Buffer *err)
{
  if (!cs || !name)
    return INT_MIN; /* LCOV_EXCL_LINE */

  struct HashElem *he = cs_get_elem(cs, name);
  if (!he)
  {
    mutt_buffer_printf(err, "Unknown var '%s'", name);
    return INT_MIN;
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
    return INT_MIN;
  }

  return cst->native_get(cs, var, vdef, err);
}

/**
 * cs_set_initial_value - Override the initial value of a config item
 * @param cs    Config items
 * @param he    HashElem representing config item
 * @param value Value to set
 * @param err   Buffer for error messages
 * @retval int Result, e.g. #CSR_SUCCESS
 */
int cs_set_initial_value(const struct ConfigSet *cs, struct HashElem *he,
                         const char *value, struct Buffer *err)
{
  if (!cs || !he)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  if (he->type & DT_INHERITED)
  {
    mutt_debug(1, "Config item %d is inherited\n", he->type);
    return CSR_ERR_CODE;
  }

  /* We've been here before */
  if (he->type & DT_INITIAL_SET)
  {
    mutt_debug(1, "Config item has already had its initial value set\n");
    return CSR_ERR_CODE;
  }

  struct VariableDef *vdef = he->data;
  if (!vdef)
    return CSR_ERR_CODE;

  if (vdef->initial != 0)
  {
    mutt_debug(1, "Config item was defined with an initial value\n");
    return CSR_ERR_CODE;
  }

  vdef->initial = IP safe_strdup(value);
  he->type |= DT_INITIAL_SET;

  return cs_reset_variable(cs, vdef->name, err);
}
