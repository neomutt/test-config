#include <stdbool.h>
#include "buffer.h"
#include "config_set.h"
#include "hash.h"
#include "lib.h"
#include "mutt_options.h"

const char *quad_values[] = { "no", "yes", "ask-no", "ask-yes" };

static bool set_quad(struct ConfigSet *set, struct HashElem *e,
                     const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_QUAD)
  {
    mutt_buffer_printf(err, "Variable is not a quad");
    return false;
  }

  for (intptr_t i = 0; i < mutt_array_size(quad_values); i++)
  {
    if (strcasecmp(quad_values[i], value) == 0)
    {
      e->data = (void *) i;
      return true;
    }
  }

  mutt_buffer_printf(err, "Invalid quad value: %s", value);
  return false;
}

static bool get_quad(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_QUAD)
  {
    mutt_buffer_printf(result, "Variable is not a quad");
    return false;
  }

  intptr_t index = (intptr_t) e->data;
  if ((index < 0) || (index > mutt_array_size(quad_values)))
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, quad_values[index]);
  return true;
}

static bool reset_quad(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_QUAD)
  {
    mutt_buffer_printf(err, "Variable is not a quad");
    return false;
  }

  return false;
}


void init_quad(void)
{
  struct ConfigSetType cst_quad = { set_quad, get_quad, reset_quad, NULL };
  cs_register_type("quad", DT_QUAD, &cst_quad);
}
