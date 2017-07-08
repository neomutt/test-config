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
typedef bool (*cs_callback)(struct ConfigSet *set, const char *name, enum ConfigEvent e);

struct ConfigSet
{
  struct ConfigSet *parent;
  struct Hash *hash;
  cs_callback cb[4];
};

struct ConfigSet *cs_new(struct ConfigSet *parent);
bool cs_init(struct ConfigSet *set, struct ConfigSet *parent);
void cs_free(struct ConfigSet *set);
void cs_add_callback(struct ConfigSet *set, cs_callback cb);
struct HashElem *cs_get_elem(struct ConfigSet *set, const char *name);

struct HashElem *cs_set_addr     (struct ConfigSet *set, const char *name, struct Address *value);
struct HashElem *cs_set_bool     (struct ConfigSet *set, const char *name, bool value);
struct HashElem *cs_set_hcache   (struct ConfigSet *set, const char *name, const char *value);
struct HashElem *cs_set_magic    (struct ConfigSet *set, const char *name, int value);
struct HashElem *cs_set_mbchartbl(struct ConfigSet *set, const char *name, struct MbCharTable *value);
struct HashElem *cs_set_num      (struct ConfigSet *set, const char *name, int value);
struct HashElem *cs_set_path     (struct ConfigSet *set, const char *name, const char *value);
struct HashElem *cs_set_quad     (struct ConfigSet *set, const char *name, int value);
struct HashElem *cs_set_rx       (struct ConfigSet *set, const char *name, struct Regex *value);
struct HashElem *cs_set_sort     (struct ConfigSet *set, const char *name, int value);
struct HashElem *cs_set_str      (struct ConfigSet *set, const char *name, const char *value);

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
