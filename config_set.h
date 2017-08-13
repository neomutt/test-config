#ifndef _MUTT_CONFIG_SET_H
#define _MUTT_CONFIG_SET_H

#include <stdbool.h>
#include <stdint.h>

struct Buffer;
struct ConfigSet;
struct HashElem;

enum ConfigEvent
{
  CE_SET,
  CE_RESET,
};

typedef bool (*cs_listener)  (struct ConfigSet *set, const char *name, enum ConfigEvent e);
typedef bool (*cs_validator) (struct ConfigSet *set, const char *name, int type, intptr_t value, struct Buffer *result);
typedef bool (*cs_destructor)(struct ConfigSet *set, unsigned int type, intptr_t obj);

typedef bool (*cst_string_set)(struct ConfigSet *set, struct HashElem *e, const char *value, struct Buffer *err);
typedef bool (*cst_string_get)(struct HashElem *e, struct Buffer *result);
typedef void (*cst_destructor)(void **obj);

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
  cst_destructor   destructor;
};

struct ConfigSet
{
  struct ConfigSet *parent;
  struct Hash *hash;
  cs_listener listeners[4];
  cs_validator validator;
};

struct ConfigSet *cs_new(struct ConfigSet *parent);
bool cs_init(struct ConfigSet *set, struct ConfigSet *parent);
void cs_free(struct ConfigSet *set);
struct HashElem *cs_get_elem(struct ConfigSet *set, const char *name);
void cs_dump_set(struct ConfigSet *set);

bool cs_register_type     (unsigned int type, struct ConfigSetType *cst);
bool cs_register_variables(struct ConfigSet *set, struct VariableDef vars[]);
struct HashElem *cs_inherit_variable(struct ConfigSet *set, struct HashElem *parent, const char *name);

void cs_add_listener (struct ConfigSet *set, cs_listener fn);
void cs_add_validator(struct ConfigSet *set, cs_validator fn);

bool cs_set_variable  (struct ConfigSet *set, const char *name, const char *value, struct Buffer *err);
bool cs_reset_variable(struct ConfigSet *set, const char *name, struct Buffer *err);
bool cs_get_variable  (struct ConfigSet *set, const char *name, struct Buffer *result);

#endif /* _MUTT_CONFIG_SET_H */
