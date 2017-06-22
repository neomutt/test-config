#ifndef _MUTT_CONFIG_SET_H
#define _MUTT_CONFIG_SET_H

#include <stdint.h>
#include "hash.h"

struct Address;
struct MbCharTbl;
struct Regex;

struct ConfigSet
{
  struct Hash *hash;
};

bool   config_set_init  (struct ConfigSet *config);
void   config_set_free  (struct ConfigSet *config);

void   config_set_addr     (struct ConfigSet *config, const char *name, struct Address *value);
void   config_set_bool     (struct ConfigSet *config, const char *name, bool value);
void   config_set_hcache   (struct ConfigSet *config, const char *name, const char *value);
void   config_set_magic    (struct ConfigSet *config, const char *name, int value);
void   config_set_mbchartbl(struct ConfigSet *config, const char *name, struct MbCharTbl *value);
void   config_set_num      (struct ConfigSet *config, const char *name, int value);
void   config_set_path     (struct ConfigSet *config, const char *name, const char *value);
void   config_set_quad     (struct ConfigSet *config, const char *name, int value);
void   config_set_rx       (struct ConfigSet *config, const char *name, struct Regex *value);
void   config_set_sort     (struct ConfigSet *config, const char *name, int value);
void   config_set_str      (struct ConfigSet *config, const char *name, const char *value);

struct Address *   config_get_addr     (struct ConfigSet *config, const char *name);
bool               config_get_bool     (struct ConfigSet *config, const char *name);
const char *       config_get_hcache   (struct ConfigSet *config, const char *name);
int                config_get_magic    (struct ConfigSet *config, const char *name);
struct MbCharTbl * config_get_mbchartbl(struct ConfigSet *config, const char *name);
int                config_get_num      (struct ConfigSet *config, const char *name);
const char *       config_get_path     (struct ConfigSet *config, const char *name);
int                config_get_quad     (struct ConfigSet *config, const char *name);
struct Regex *     config_get_rx       (struct ConfigSet *config, const char *name);
int                config_get_sort     (struct ConfigSet *config, const char *name);
const char *       config_get_str      (struct ConfigSet *config, const char *name);

#endif /* _MUTT_CONFIG_SET_H */
