#ifndef _MUTT_CONFIG_SET_H
#define _MUTT_CONFIG_SET_H

#include <stdint.h>
#include "hash.h"

struct Address;
struct MbCharTable;
struct Regex;

enum ConfigEvent
{
  CE_SET,
  CE_CHANGED,
};

struct ConfigSet;
typedef bool (*config_callback)(struct ConfigSet *set, const char *name, enum ConfigEvent e);

struct ConfigSet
{
  struct ConfigSet *parent;
  struct Hash *hash;
  config_callback cb[4];
};

struct ConfigSet *set_set_new(struct ConfigSet *parent);
bool config_set_init(struct ConfigSet *set, struct ConfigSet *parent);
void config_set_free(struct ConfigSet *set);
void config_set_add_callback(struct ConfigSet *set, config_callback cb);
struct HashElem *config_get_var(struct ConfigSet *set, const char *name);

struct HashElem *config_set_addr     (struct ConfigSet *set, const char *name, struct Address *value);
struct HashElem *config_set_bool     (struct ConfigSet *set, const char *name, bool value);
struct HashElem *config_set_hcache   (struct ConfigSet *set, const char *name, const char *value);
struct HashElem *config_set_magic    (struct ConfigSet *set, const char *name, int value);
struct HashElem *config_set_mbchartbl(struct ConfigSet *set, const char *name, struct MbCharTable *value);
struct HashElem *config_set_num      (struct ConfigSet *set, const char *name, int value);
struct HashElem *config_set_path     (struct ConfigSet *set, const char *name, const char *value);
struct HashElem *config_set_quad     (struct ConfigSet *set, const char *name, int value);
struct HashElem *config_set_rx       (struct ConfigSet *set, const char *name, struct Regex *value);
struct HashElem *config_set_sort     (struct ConfigSet *set, const char *name, int value);
struct HashElem *config_set_str      (struct ConfigSet *set, const char *name, const char *value);

struct Address *     config_get_addr     (struct ConfigSet *set, const char *name);
bool                 config_get_bool     (struct ConfigSet *set, const char *name);
const char *         config_get_hcache   (struct ConfigSet *set, const char *name);
int                  config_get_magic    (struct ConfigSet *set, const char *name);
struct MbCharTable * config_get_mbchartbl(struct ConfigSet *set, const char *name);
int                  config_get_num      (struct ConfigSet *set, const char *name);
const char *         config_get_path     (struct ConfigSet *set, const char *name);
int                  config_get_quad     (struct ConfigSet *set, const char *name);
struct Regex *       config_get_rx       (struct ConfigSet *set, const char *name);
int                  config_get_sort     (struct ConfigSet *set, const char *name);
const char *         config_get_str      (struct ConfigSet *set, const char *name);

struct HashElem *var_set_addr     (struct HashElem *var, struct Address *value);
struct HashElem *var_set_bool     (struct HashElem *var, bool value);
struct HashElem *var_set_hcache   (struct HashElem *var, const char *value);
struct HashElem *var_set_magic    (struct HashElem *var, int value);
struct HashElem *var_set_mbchartbl(struct HashElem *var, struct MbCharTable *value);
struct HashElem *var_set_num      (struct HashElem *var, int value);
struct HashElem *var_set_path     (struct HashElem *var, const char *value);
struct HashElem *var_set_quad     (struct HashElem *var, int value);
struct HashElem *var_set_rx       (struct HashElem *var, struct Regex *value);
struct HashElem *var_set_sort     (struct HashElem *var, int value);
struct HashElem *var_set_str      (struct HashElem *var, const char *value);

struct Address *     var_get_addr     (struct HashElem *var);
bool                 var_get_bool     (struct HashElem *var);
const char *         var_get_hcache   (struct HashElem *var);
int                  var_get_magic    (struct HashElem *var);
struct MbCharTable * var_get_mbchartbl(struct HashElem *var);
int                  var_get_num      (struct HashElem *var);
const char *         var_get_path     (struct HashElem *var);
int                  var_get_quad     (struct HashElem *var);
struct Regex *       var_get_rx       (struct HashElem *var);
int                  var_get_sort     (struct HashElem *var);
const char *         var_get_str      (struct HashElem *var);

#endif /* _MUTT_CONFIG_SET_H */
