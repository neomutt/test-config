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

bool config_set_addr     (struct ConfigSet *set, const char *name, struct Address *value);
bool config_set_bool     (struct ConfigSet *set, const char *name, bool value);
bool config_set_hcache   (struct ConfigSet *set, const char *name, const char *value);
bool config_set_magic    (struct ConfigSet *set, const char *name, int value);
bool config_set_mbchartbl(struct ConfigSet *set, const char *name, struct MbCharTable *value);
bool config_set_num      (struct ConfigSet *set, const char *name, int value);
bool config_set_path     (struct ConfigSet *set, const char *name, const char *value);
bool config_set_quad     (struct ConfigSet *set, const char *name, int value);
bool config_set_rx       (struct ConfigSet *set, const char *name, struct Regex *value);
bool config_set_sort     (struct ConfigSet *set, const char *name, int value);
bool config_set_str      (struct ConfigSet *set, const char *name, const char *value);

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

#endif /* _MUTT_CONFIG_SET_H */
