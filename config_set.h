#ifndef _MUTT_CONFIG_SET_H
#define _MUTT_CONFIG_SET_H

#include <stdint.h>
#include "hash.h"

struct Address;
struct Buffer;
struct MbCharTable;
struct Regex;

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
typedef bool (*cs_type_destructor)(struct HashElem *e, struct Buffer *err);

struct VariableDef
{
  const char  *name;
  int          type;
  intptr_t     variable;
  intptr_t     initial;
  cs_validator validator;
};

struct ConfigSetType
{
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
  cs_destructor destructor;
};

struct ConfigSet *cs_new(struct ConfigSet *parent);
bool cs_init(struct ConfigSet *set, struct ConfigSet *parent);
void cs_free(struct ConfigSet *set);
struct HashElem *cs_get_elem(struct ConfigSet *set, const char *name);
void cs_dump_set(struct ConfigSet *set);

bool cs_register_type     (const char *name, int type_id, struct ConfigSetType *cst);
bool cs_register_variable (const char *name, int type_id, const char *initial, cs_validator validator);
bool cs_register_variables(struct ConfigSet *set, struct VariableDef vars[]);

void cs_add_listener  (struct ConfigSet *set, cs_listener fn);
void cs_add_validator (struct ConfigSet *set, cs_validator fn);
void cs_add_destructor(struct ConfigSet *set, cs_destructor fn);

struct HashElem *cs_set_addr     (struct ConfigSet *set, const char *name, struct Address *value,     struct Buffer *result);
struct HashElem *cs_set_bool     (struct ConfigSet *set, const char *name, bool value,                struct Buffer *result);
struct HashElem *cs_set_hcache   (struct ConfigSet *set, const char *name, const char *value,         struct Buffer *result);
struct HashElem *cs_set_magic    (struct ConfigSet *set, const char *name, int value,                 struct Buffer *result);
struct HashElem *cs_set_mbchartbl(struct ConfigSet *set, const char *name, struct MbCharTable *value, struct Buffer *result);
struct HashElem *cs_set_num      (struct ConfigSet *set, const char *name, int value,                 struct Buffer *result);
struct HashElem *cs_set_path     (struct ConfigSet *set, const char *name, const char *value,         struct Buffer *result);
struct HashElem *cs_set_quad     (struct ConfigSet *set, const char *name, int value,                 struct Buffer *result);
struct HashElem *cs_set_rx       (struct ConfigSet *set, const char *name, struct Regex *value,       struct Buffer *result);
struct HashElem *cs_set_sort     (struct ConfigSet *set, const char *name, int value,                 struct Buffer *result);
struct HashElem *cs_set_str      (struct ConfigSet *set, const char *name, const char *value,         struct Buffer *result);

struct Address *    cs_get_addr     (struct ConfigSet *set, const char *name);
bool                cs_get_bool     (struct ConfigSet *set, const char *name);
const char *        cs_get_hcache   (struct ConfigSet *set, const char *name);
int                 cs_get_magic    (struct ConfigSet *set, const char *name);
struct MbCharTable *cs_get_mbchartbl(struct ConfigSet *set, const char *name);
int                 cs_get_num      (struct ConfigSet *set, const char *name);
const char *        cs_get_path     (struct ConfigSet *set, const char *name);
int                 cs_get_quad     (struct ConfigSet *set, const char *name);
struct Regex *      cs_get_rx       (struct ConfigSet *set, const char *name);
int                 cs_get_sort     (struct ConfigSet *set, const char *name);
const char *        cs_get_str      (struct ConfigSet *set, const char *name);

struct HashElem *he_set_addr     (struct HashElem *e, struct Address *value);
struct HashElem *he_set_bool     (struct HashElem *e, bool value);
struct HashElem *he_set_hcache   (struct HashElem *e, const char *value);
struct HashElem *he_set_magic    (struct HashElem *e, int value);
struct HashElem *he_set_mbchartbl(struct HashElem *e, struct MbCharTable *value);
struct HashElem *he_set_num      (struct HashElem *e, int value);
struct HashElem *he_set_path     (struct HashElem *e, const char *value);
struct HashElem *he_set_quad     (struct HashElem *e, int value);
struct HashElem *he_set_rx       (struct HashElem *e, struct Regex *value);
struct HashElem *he_set_sort     (struct HashElem *e, int value);
struct HashElem *he_set_str      (struct HashElem *e, const char *value);

struct Address *     he_get_addr     (struct HashElem *e);
bool                 he_get_bool     (struct HashElem *e);
const char *         he_get_hcache   (struct HashElem *e);
int                  he_get_magic    (struct HashElem *e);
struct MbCharTable * he_get_mbchartbl(struct HashElem *e);
int                  he_get_num      (struct HashElem *e);
const char *         he_get_path     (struct HashElem *e);
int                  he_get_quad     (struct HashElem *e);
struct Regex *       he_get_rx       (struct HashElem *e);
int                  he_get_sort     (struct HashElem *e);
const char *         he_get_str      (struct HashElem *e);

#endif /* _MUTT_CONFIG_SET_H */
