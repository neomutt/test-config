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

typedef bool     (*cs_listener)  (struct ConfigSet *cs, struct HashElem *he, const char *name, enum ConfigEvent e);
typedef bool     (*cs_validator) (const struct ConfigSet *cs, const struct VariableDef *def, intptr_t value, struct Buffer *result);

typedef bool     (*cst_string_set)(struct ConfigSet *cs, void *var, const struct VariableDef *def, const char *value, struct Buffer *err);
typedef bool     (*cst_string_get)(void *var, const struct VariableDef *def, struct Buffer *result);
typedef bool     (*cst_reset)     (struct ConfigSet *cs, void *var, const struct VariableDef *def, struct Buffer *err);
typedef void     (*cst_destructor)(void *var, const struct VariableDef *def);

typedef bool     (*cst_native_set)(struct ConfigSet *cs, void *var, const struct VariableDef *def, intptr_t value, struct Buffer *err);
typedef intptr_t (*cst_native_get)(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, struct Buffer *err);

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
  cst_reset        resetter;
  cst_destructor   destructor;
  cst_native_set   nsetter;
  cst_native_get   ngetter;
};

struct ConfigSet
{
  struct Hash *hash;
  cs_listener listeners[4];
  cs_validator validator;
};

struct ConfigSet *cs_new(struct ConfigSet *parent, int size);
bool cs_init(struct ConfigSet *cs, struct ConfigSet *parent, int size);
void cs_free(struct ConfigSet *cs);
struct HashElem *cs_get_elem(struct ConfigSet *cs, const char *name);
void cs_dump_set(struct ConfigSet *cs);

bool cs_register_type     (unsigned int type, const struct ConfigSetType *cst);
bool cs_register_variables(struct ConfigSet *cs, const struct VariableDef vars[]);
struct HashElem *cs_inherit_variable(struct ConfigSet *cs, struct HashElem *parent, const char *name);

void cs_add_listener (struct ConfigSet *cs, cs_listener fn);
void cs_add_validator(struct ConfigSet *cs, cs_validator fn);

bool cs_set_variable  (struct ConfigSet *cs, const char *name, const char *value, struct Buffer *err);
bool cs_reset_variable(struct ConfigSet *cs, const char *name, struct Buffer *err);
bool cs_get_variable  (struct ConfigSet *cs, const char *name, struct Buffer *result);

void notify_listeners(struct ConfigSet *cs, struct HashElem *he, const char *name, enum ConfigEvent ev);

bool cs_set_value(struct ConfigSet *cs, struct HashElem *he, intptr_t value, struct Buffer *err);
bool cs_get_value(struct ConfigSet *cs, struct HashElem *he, struct Buffer *err);

bool cs_set_value2(struct ConfigSet *cs, const char *name, intptr_t value, struct Buffer *err);
bool cs_get_value2(struct ConfigSet *cs, const char *name, struct Buffer *err);

#endif /* _MUTT_CONFIG_SET_H */
