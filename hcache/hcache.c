#include "hcache.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "config_set.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "mutt_options.h"

const char *hcache_backends[] = {
  "tokyocabinet", "kyotocabinet", "qdbm", "gdbm", "bdb", "lmdb",
};

static bool set_hcache(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef, const char *value, struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return false;

  intptr_t index = -1;

  for (unsigned int i = 0; i < mutt_array_size(hcache_backends); i++)
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

  *(short *) var = index;
  return true;
}

static bool get_hcache(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false;

  unsigned int index = *(short *) var;
  if (index >= mutt_array_size(hcache_backends))
  {
    mutt_buffer_printf(result, "Invalid hcache value: %d", index);
    return NULL;
  }

  mutt_buffer_addstr(result, hcache_backends[index]);
  return true;
}

static bool set_native_hcache(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  if ((value < 0) || (value >= mutt_array_size(hcache_backends)))
  {
    mutt_buffer_printf(err, "Invalid hcache: %ld", value);
    return false;
  }

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  *(short *) var = value;
  return true;
}

static intptr_t get_native_hcache(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  return *(short *) var;
}

static bool reset_hcache(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  *(short *) var = vdef->initial;
  return true;
}

// static
bool hc_pagesize_validator(const struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  if (!cs || !vdef || !value)
    return false;

  int num = 0;
  if (mutt_atoi((const char*) value, &num) < 0)
  {
    mutt_buffer_printf(err, "Invalid number: %s", value);
    return false;
  }

  if (num < 512)
  {
    mutt_buffer_printf(err, "Number is too small: %d", num);
    return false;
  }

  if (num & (num - 1))
  {
    mutt_buffer_printf(err, "Number is not a power of two: %d", num);
    return false;
  }

  return true;
}

char *HeaderCache;
short HeaderCacheBackend;
char *HeaderCachePageSize;
bool  OPT_HCACHE_COMPRESS;

struct VariableDef HCVars[] = {
  // { "header_cache",          DT_PATH,   &HeaderCache,         0 },
  { "header_cache_backend",  DT_HCACHE, &HeaderCacheBackend,  0 },
  // { "header_cache_compress", DT_BOOL,   &OPT_HCACHE_COMPRESS, 1 },
  // { "header_cache_pagesize", DT_STR,    &HeaderCachePageSize, IP "16384", hc_pagesize_validator },
  { NULL },
};

void init_hcache(struct ConfigSet *cs)
{
  const struct ConfigSetType cst = { "hcache", set_hcache, get_hcache, set_native_hcache, get_native_hcache, reset_hcache, NULL, };

  cs_register_type(cs, DT_HCACHE, &cst);

  cs_register_variables(cs, HCVars);
}

