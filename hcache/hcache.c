#include "hcache.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "buffer.h"
#include "config_set.h"
#include "hash.h"
#include "lib.h"
#include "mutt_options.h"

const char *hcache_backends[] = {
  "tokyocabinet", "kyotocabinet", "qdbm", "gdbm", "bdb", "lmdb",
};


static bool hc_string_set(struct ConfigSet *set, struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_HCACHE)
  {
    mutt_buffer_printf(err, "Variable is not an hcache");
    return false;
  }

  intptr_t index = -1;

  for (int i = 0; i < mutt_array_size(hcache_backends); i++)
  {
    if (strcmp(value, hcache_backends[i]) == 0)
    {
      index = i;
      break;
    }
  }

  if (index < 0)
  {
    mutt_buffer_printf(err, "Not a valid hcache backend: %s", value);
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  *(short *) v->variable = index;
  return true;
}

static bool hc_string_get(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_HCACHE)
  {
    mutt_buffer_printf(result, "Variable is not an hcache");
    return NULL;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  int index = *(short *) v->variable;
  if ((index < 0) || (index >= mutt_array_size(hcache_backends)))
  {
    mutt_buffer_printf(result, "Invalid hcache value: %d", index);
    return NULL;
  }

  mutt_buffer_addstr(result, hcache_backends[index]);
  return true;
}


static bool hc_pagesize_validator(struct ConfigSet *set, const char *name, int type, intptr_t value, struct Buffer *result)
{
  int x = 1024;

  if (x < 512)
    return false;

  // Power of two
  return (!(x & (x - 1)));
}

char *HeaderCache;
short HeaderCacheBackend;
char *HeaderCachePageSize;
bool  OPT_HCACHE_COMPRESS;

struct VariableDef HCVars[] = {
  { "header_cache",          DT_PATH,   &HeaderCache,         0 },
  { "header_cache_backend",  DT_HCACHE, &HeaderCacheBackend,  0 },
  { "header_cache_compress", DT_BOOL,   &OPT_HCACHE_COMPRESS, 1 },
  { "header_cache_pagesize", DT_STR,    &HeaderCachePageSize, IP "16384", hc_pagesize_validator },
  { NULL },
};

void init_hcache(struct ConfigSet *set)
{
  struct ConfigSetType cst = { "hcache", hc_string_set, hc_string_get, NULL, NULL };

  cs_register_type(DT_HCACHE, &cst);

  cs_register_variables(set, HCVars);
}

