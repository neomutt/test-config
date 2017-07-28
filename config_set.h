#ifndef _MUTT_CONFIG_SET_H
#define _MUTT_CONFIG_SET_H

#include <stdint.h>
#include "hash.h"

struct Buffer;

enum ConfigEvent
{
  CE_SET,
  CE_CHANGED,
};

struct ConfigSet;
typedef bool (*cs_listener)  (struct ConfigSet *set, const char *name, enum ConfigEvent e);
typedef bool (*cs_validator) (struct ConfigSet *set, const char *name, int type, intptr_t value, struct Buffer *result);
typedef bool (*cs_destructor)(struct ConfigSet *set, int type, intptr_t obj);

typedef bool (*cs_type_string_set)(struct ConfigSet *set, struct HashElem *e, const char *value, struct Buffer *err);
typedef bool (*cs_type_string_get)(struct HashElem *e, struct Buffer *result);
typedef bool (*cs_type_reset)     (struct ConfigSet *set, struct HashElem *e, struct Buffer *err);
typedef void (*cs_type_destructor)(void **obj);

struct VariableDef
{
  const char  *name;
  int          type;
  void        *variable;
  intptr_t     initial;
  cs_validator validator;
};

struct ConfigSetType
{
  const char *name;
  cs_type_string_set   setter;
  cs_type_string_get   getter;
  cs_type_reset        resetter;
  cs_type_destructor   destructor;
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

bool cs_register_type     (int type_id, struct ConfigSetType *cst);
bool cs_register_variables(struct ConfigSet *set, struct VariableDef vars[]);

void cs_add_listener (struct ConfigSet *set, cs_listener fn);
void cs_add_validator(struct ConfigSet *set, cs_validator fn);

#endif /* _MUTT_CONFIG_SET_H */
