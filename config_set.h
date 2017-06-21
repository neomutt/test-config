#ifndef _MUTT_CONFIG_SET_H
#define _MUTT_CONFIG_SET_H

#include <stdint.h>
#include "hash.h"

struct ConfigSet
{
  struct Hash *hash;
};

bool   config_set_init  (struct ConfigSet *config);
void   config_set_free  (struct ConfigSet *config);

void   config_set_int   (struct ConfigSet *config, const char *name, intptr_t value);
void   config_set_string(struct ConfigSet *config, const char *name, const char *value);
int    config_get_int   (struct ConfigSet *config, const char *name);
char * config_get_string(struct ConfigSet *config, const char *name);

#endif /* _MUTT_CONFIG_SET_H */
