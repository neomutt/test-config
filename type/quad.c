#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

const char *quad_values[] = { "no", "yes", "ask-no", "ask-yes" };

static bool set_quad(struct ConfigSet *cs, void *var, const struct VariableDef *vdef,
                     const char *value, struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return false;

  for (unsigned int i = 0; i < mutt_array_size(quad_values); i++)
  {
    if (mutt_strcasecmp(quad_values[i], value) == 0)
    {
      *(short *) var = i;
      return true;
    }
  }

  mutt_buffer_printf(err, "Invalid quad value: %s", value);
  return false;
}

static bool get_quad(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false;

  unsigned int index = *(short *) var;
  if (index >= mutt_array_size(quad_values))
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, quad_values[index]);
  return true;
}

static bool reset_quad(struct ConfigSet *cs, void *var,
                       const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  *(short *) var = vdef->initial;
  return true;
}

void init_quad(void)
{
  const struct ConfigSetType cst_quad = { "quad", set_quad, get_quad, reset_quad, NULL };
  cs_register_type(DT_QUAD, &cst_quad);
}
