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

#ifndef _CONFIG_SET_H
#define _CONFIG_SET_H

#include <stdbool.h>
#include <stdint.h>

struct Buffer;
struct ConfigSet;
struct HashElem;
struct ConfigDef;

/**
 * enum ConfigEvent - XXX
 */
enum ConfigEvent
{
  CE_SET,   /**< XXX */
  CE_RESET, /**< XXX */
};

/* Config Set Results */
#define CSR_SUCCESS       0 /**< Action completed successfully */
#define CSR_ERR_CODE      1 /**< Problem with the code */
#define CSR_ERR_UNKNOWN   2 /**< Unrecognised config item */
#define CSR_ERR_INVALID   3 /**< Value hasn't been set */
/* Flags for CSR_SUCCESS */
#define CSR_SUC_INHERITED (1 << 4) /**< Value is inherited */
#define CSR_SUC_EMPTY     (1 << 5) /**< Value is empty/unset */
#define CSR_SUC_WARNING   (1 << 6) /**< Notify the user of a warning */
/* Flags for CSR_INVALID */
#define CSR_INV_TYPE      (1 << 4) /**< Value is not valid for the type */
#define CSR_INV_VALIDATOR (1 << 5) /**< Value was rejected by the validator */

#define CSR_RESULT_MASK 0x0F
#define CSR_RESULT(x) ((x) & CSR_RESULT_MASK)

/**
 * enum CsListenerAction - XXX
 */
enum CsListenerAction
{
  CSLA_CONTINUE, /**< XXX */
  CSLA_STOP,     /**< XXX */
};

typedef bool    (*cs_listener)   (const struct ConfigSet *cs, struct HashElem *he, const char *name, enum ConfigEvent ev);
typedef int     (*cs_validator)  (const struct ConfigSet *cs, const struct ConfigDef *cdef, intptr_t value, struct Buffer *result);

typedef int     (*cst_string_set)(const struct ConfigSet *cs, void *var, const struct ConfigDef *cdef, const char *value, struct Buffer *err);
typedef int     (*cst_string_get)(const struct ConfigSet *cs, void *var, const struct ConfigDef *cdef, struct Buffer *result);
typedef int     (*cst_native_set)(const struct ConfigSet *cs, void *var, const struct ConfigDef *cdef, intptr_t value, struct Buffer *err);
typedef intptr_t(*cst_native_get)(const struct ConfigSet *cs, void *var, const struct ConfigDef *cdef, struct Buffer *err);

typedef int     (*cst_reset)     (const struct ConfigSet *cs, void *var, const struct ConfigDef *cdef, struct Buffer *err);
typedef void    (*cst_destroy)   (const struct ConfigSet *cs, void *var, const struct ConfigDef *cdef);

#define IP (intptr_t)

#define CS_REG_DISABLED (1 << 0)

/**
 * struct ConfigDef - XXX
 */
struct ConfigDef
{
  const char   *name;      /**< user-visible name */
  unsigned int  type;      /**< variable type, e.g. *DT_STRING */
  short         flags;     /**< notification flags, e.g. R_PAGER */
  void         *var;       /**< pointer to the global variable */
  intptr_t      initial;   /**< initial value */
  cs_validator  validator; /**< validator callback function */
};

/**
 * struct ConfigSetType - XXX
 */
struct ConfigSetType
{
  const char *name;          /**< XXX */
  cst_string_set string_set; /**< XXX */
  cst_string_get string_get; /**< XXX */
  cst_native_set native_set; /**< XXX */
  cst_native_get native_get; /**< XXX */
  cst_reset reset;           /**< XXX */
  cst_destroy destroy;       /**< XXX */
};

/**
 * struct ConfigSet - XXX
 */
struct ConfigSet
{
  struct Hash *hash;              /**< XXX */
  struct ConfigSetType types[14]; /**< XXX */
  cs_listener listeners[4];       /**< XXX */
};

struct ConfigSet *cs_create(int size);
void cs_init(struct ConfigSet *cs, int size);
void cs_free(struct ConfigSet **cs);

struct HashElem *cs_get_elem(const struct ConfigSet *cs, const char *name);
const struct ConfigSetType *cs_get_type_def(const struct ConfigSet *cs, unsigned int type);

bool cs_register_type(struct ConfigSet *cs, unsigned int type, const struct ConfigSetType *cst);
bool cs_register_variables(const struct ConfigSet *cs, struct ConfigDef vars[], int flags);
struct HashElem *cs_inherit_variable(const struct ConfigSet *cs, struct HashElem *parent, const char *name);
int cs_set_initial_value(const struct ConfigSet *cs, const char *name, const char *value, struct Buffer *err);

void cs_add_listener(struct ConfigSet *cs, cs_listener fn);
void cs_remove_listener(struct ConfigSet *cs, cs_listener fn);
void cs_notify_listeners(const struct ConfigSet *cs, struct HashElem *he, const char *name, enum ConfigEvent ev);

int cs_str_reset(const struct ConfigSet *cs, const char *name, struct Buffer *err);
int cs_he_reset(const struct ConfigSet *cs, struct HashElem *he, struct Buffer *err);

int cs_he_string_get(const struct ConfigSet *cs, struct HashElem *he, struct Buffer *result);
int cs_he_default_get(const struct ConfigSet *cs, struct HashElem *he, struct Buffer *result);

int cs_str_string_set(const struct ConfigSet *cs, const char *name, const char *value, struct Buffer *err);
int cs_str_string_get(const struct ConfigSet *cs, const char *name, struct Buffer *result);
int cs_str_default_get(const struct ConfigSet *cs, const char *name, struct Buffer *result);

int cs_he_string_set(const struct ConfigSet *cs, struct HashElem *he, const char *value, struct Buffer *err);

int cs_he_native_set(const struct ConfigSet *cs, struct HashElem *he, intptr_t value, struct Buffer *err);
intptr_t cs_he_native_get(const struct ConfigSet *cs, struct HashElem *he, struct Buffer *err);

int      cs_str_native_set(const struct ConfigSet *cs, const char *name, intptr_t value, struct Buffer *err);
intptr_t cs_str_native_get(const struct ConfigSet *cs, const char *name, struct Buffer *err);

#endif /* _CONFIG_SET_H */
