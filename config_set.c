#include "config.h"
#include <limits.h>
#include <stdlib.h>
#include "config_set.h"
#include "hash.h"

bool config_set_init(struct ConfigSet *config)
{
  config->hash = hash_create(10, 0);
  return true;
}

void config_set_free(struct ConfigSet *config)
{
  hash_destroy(&config->hash, NULL);
}

void config_set_int(struct ConfigSet *config, const char *name, intptr_t value)
{
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
  {
    elem->data = (void *) value;
    return;
  }

  hash_insert(config->hash, name, (void *) value);
}

void config_set_string(struct ConfigSet *config, const char *name, const char *value)
{
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
  {
    elem->data = (void *) value;
    return;
  }

  hash_insert(config->hash, name, (void *) value);
}

int config_get_int(struct ConfigSet *config, const char *name)
{
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
    return (intptr_t) elem->data;

  return INT_MIN;
}

char *config_get_string(struct ConfigSet *config, const char *name)
{
  struct HashElem *elem = hash_find_elem(config->hash, name);
  if (elem)
    return elem->data;

  return NULL;
}
