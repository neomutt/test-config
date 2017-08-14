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

typedef bool (*cs_listener)  (struct ConfigSet *cs, const char *name, enum ConfigEvent e);
typedef bool (*cs_validator) (struct ConfigSet *cs, const struct VariableDef *def, intptr_t value, struct Buffer *result);

typedef bool (*cst_string_set)(struct ConfigSet *cs, void *variable, const struct VariableDef *def, const char *value, struct Buffer *err);
typedef bool (*cst_string_get)(void *variable, const struct VariableDef *def, struct Buffer *result);
typedef bool (*cst_reset)     (struct ConfigSet *cs, void *variable, const struct VariableDef *def, struct Buffer *err);
typedef void (*cst_destructor)(void **variable, const struct VariableDef *def);

#define IP (intptr_t)

struct VariableDef
{
  const char   *name;
  unsigned int  type;
  void         *variable;
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
};

struct ConfigSet
{
  struct Hash *hash;
  cs_listener listeners[4];
  cs_validator validator;
};

struct ConfigSet *cs_new(struct ConfigSet *parent);
bool cs_init(struct ConfigSet *cs, struct ConfigSet *parent);
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

#endif /* _MUTT_CONFIG_SET_H */
