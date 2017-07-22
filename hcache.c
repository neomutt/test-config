#include <string.h>
#include "hcache.h"
#include "buffer.h"
#include "config_set.h"
#include "lib.h"
#include "mutt_options.h"

const char *hcache_backends[] = {
  "tokyocabinet", "kyotocabinet", "qdbm", "gdbm", "bdb", "lmdb",
};


static bool hc_string_set(struct HashElem *e, const char *value, struct Buffer *err)
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

  e->data = (void *) index;
  return true;
}

static const char *hc_string_get(struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_HCACHE)
  {
    mutt_buffer_printf(err, "Variable is not an hcache");
    return NULL;
  }

  int index = (intptr_t) e->data;
  if ((index < 0) || (index >= mutt_array_size(hcache_backends)))
  {
    mutt_buffer_printf(err, "Invalid hcache value: %d", index);
    return NULL;
  }

  return hcache_backends[index];
}

static bool hc_validator(const char *name, int type, intptr_t value, struct Buffer *err)
{
  // validate an hcache string
  return true;
}

static bool hc_destructor(struct HashElem *e, struct Buffer *err)
{
  /* We only store an int, so no action is required */
  return true;
}


bool hcache_init(void)
{
  struct ConfigSetType cst = { hc_string_set, hc_string_get, hc_validator, hc_destructor };

  cs_register_type("hcache", DT_HCACHE, &cst);

  return true;
}
