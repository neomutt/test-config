#include "hcache.h"
#include "config_set.h"
#include "mutt_options.h"

static bool string_set(struct HashElem *e, const char *value, struct Buffer *err)
{
  // Set a hcache by string
  return true;
}

static const char *string_get(struct HashElem *e, struct Buffer *err)
{
  // get a hcache as a string
  return "lmdb";
}

static bool validator(const char *name, int type, intptr_t value, struct Buffer *err)
{
  // validate an hcache string
  return true;
}

static bool destructor(struct HashElem *e, struct Buffer *err)
{
  // destroy an hcache string
  return true;
}

bool hcache_init(void)
{
  struct ConfigSetType cst = { string_set, string_get, validator, destructor };

  cs_register_type("hcache", DT_HCACHE, &cst);

  return true;
}

