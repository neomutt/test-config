#include <stdbool.h>
#include "buffer.h"
#include "config_set.h"
#include "hash.h"
#include "lib.h"
#include "mutt_options.h"

static void str_destructor(void **obj)
{
  if (!obj || !*obj)
    return;

  FREE(obj);
}

static bool set_str(struct ConfigSet *set, struct HashElem *e,
                    const char *value, struct Buffer *err)
{
  if (e && DTYPE(e->type) != DT_STR)
  {
    mutt_buffer_printf(err, "Variable is not a string");
    return false;
  }

  // if (e)
  // {
  //   if (!str_destructor(e, err))
  //     return false;

  //   e->data = (void *) value;
  // }
  // else
  // {
  //   e = hash_typed_insert(hash, name, DT_STR, (void *) value);
  // }

  return false;
}

static bool get_str(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_STR)
  {
    mutt_buffer_printf(result, "Variable is not a string");
    return false;
  }

  mutt_buffer_addstr(result, (const char *) e->data);
  return true;
}

static bool reset_str(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (e && DTYPE(e->type) != DT_STR)
  {
    mutt_buffer_printf(err, "Variable is not a string");
    return false;
  }

  // if (e)
  // {
  //   if (!str_destructor(e, err))
  //     return false;

  //   e->data = (void *) value;
  // }
  // else
  // {
  //   e = hash_typed_insert(hash, name, DT_STR, (void *) value);
  // }

  return false;
}


void init_string(void)
{
  struct ConfigSetType cst_str = { set_str, get_str, reset_str, str_destructor };
  cs_register_type("string", DT_STR, &cst_str);
}
