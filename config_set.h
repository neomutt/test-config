#ifndef _MUTT_CONFIG_SET_H
#define _MUTT_CONFIG_SET_H

#include <stdbool.h>
#include <stdint.h>

struct Buffer;
struct ConfigSet;
struct HashElem;
struct VariableDef;

enum ConfigEvent
{
  CE_SET,
  CE_RESET,
};

typedef bool     (*cs_listener)  (const struct ConfigSet *cs, struct HashElem *he, const char *name, enum ConfigEvent ev);
typedef bool     (*cs_validator) (const struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *result);

typedef bool     (*cst_string_set)(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef, const char *value, struct Buffer *err);
typedef bool     (*cst_string_get)(void *var, const struct VariableDef *vdef, struct Buffer *result);
typedef bool     (*cst_native_set)(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef, intptr_t value, struct Buffer *err);
typedef intptr_t (*cst_native_get)(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef, struct Buffer *err);
typedef bool     (*cst_reset)     (const struct ConfigSet *cs, void *var, const struct VariableDef *vdef, struct Buffer *err);
typedef void     (*cst_destructor)(void *var, const struct VariableDef *vdef);

#define IP (intptr_t)

struct VariableDef
{
  const char   *name;
  unsigned int  type;
  void         *var;
  intptr_t      initial;
  cs_validator  validator;
};

struct ConfigSetType
{
  const char *name;
  cst_string_set   setter;
  cst_string_get   getter;
  cst_native_set   nsetter;
  cst_native_get   ngetter;
  cst_reset        resetter;
  cst_destructor   destructor;
};

struct ConfigSet
{
  struct Hash *hash;
  struct ConfigSetType types[16];
  cs_listener listeners[4];
};

struct ConfigSet *cs_new_set(int size);
void cs_init(struct ConfigSet *cs, int size);
void cs_free(struct ConfigSet **cs);

struct HashElem *cs_get_elem(const struct ConfigSet *cs, const char *name);
const struct ConfigSetType *cs_get_type_def(const struct ConfigSet *cs, unsigned int type);

bool cs_register_type     (struct ConfigSet *cs, unsigned int type, const struct ConfigSetType *cst);
bool cs_register_variables(const struct ConfigSet *cs, const struct VariableDef vars[]);
struct HashElem *cs_inherit_variable(const struct ConfigSet *cs, struct HashElem *parent, const char *name);

void cs_add_listener (struct ConfigSet *cs, cs_listener fn);
//XXX void cs_remove_listener (struct ConfigSet *cs, cs_listener fn);

bool cs_set_variable  (const struct ConfigSet *cs, const char *name, const char *value, struct Buffer *err);
bool cs_reset_variable(const struct ConfigSet *cs, const char *name, struct Buffer *err);
bool cs_get_variable  (const struct ConfigSet *cs, const char *name, struct Buffer *result);
bool cs_get_variable2 (const struct ConfigSet *cs, const struct HashElem *he, struct Buffer *result);

void notify_listeners(const struct ConfigSet *cs, struct HashElem *he, const char *name, enum ConfigEvent ev);

bool cs_he_set_value(const struct ConfigSet *cs, struct HashElem *he, intptr_t value, struct Buffer *err);
bool cs_he_get_value(const struct ConfigSet *cs, struct HashElem *he, struct Buffer *err);

bool     cs_str_set_value(const struct ConfigSet *cs, const char *name, intptr_t value, struct Buffer *err);
intptr_t cs_str_get_value(const struct ConfigSet *cs, const char *name, struct Buffer *err);

bool cs_set_initial_value(const struct ConfigSet *cs, struct HashElem *he, const char *value, struct Buffer *err);

#endif /* _MUTT_CONFIG_SET_H */
