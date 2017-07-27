#include <stdbool.h>
#include "buffer.h"
#include "hash.h"
#include "config_set.h"
#include "mutt_options.h"
#include "lib.h"

const char *bool_values[] = { "no", "yes" };

static bool set_bool(struct ConfigSet *set, struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_BOOL)
  {
    mutt_buffer_printf(err, "Variable is not a boolean");
    return false;
  }

  for (intptr_t i = 0; i < mutt_array_size(bool_values); i++)
  {
    if (strcasecmp(bool_values[i], value) == 0)
    {
      e->data = (void *) i;
      return true;
    }
  }

  mutt_buffer_printf(err, "Invalid boolean value: %s", value);
  return false;
}

static bool get_bool(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_BOOL)
  {
    mutt_buffer_printf(result, "Variable is not an address");
    return false;
  }

  intptr_t index = (intptr_t) e->data;
  if ((index < 0) || (index > mutt_array_size(bool_values)))
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, bool_values[index]);
  return true;
}

static bool reset_bool(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_BOOL)
  {
    mutt_buffer_printf(err, "Variable is not an boolean");
    return false;
  }

  return false;
}


void init_bool(void)
{
  struct ConfigSetType cst_bool = { set_bool, get_bool, reset_bool, NULL };
  cs_register_type("boolean", DT_BOOL, &cst_bool);
}
