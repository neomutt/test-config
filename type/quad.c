#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

const char *quad_values[] = { "no", "yes", "ask-no", "ask-yes" };

static bool set_quad(struct ConfigSet *cs, void *variable, const struct VariableDef *vdef,
                     const char *value, struct Buffer *err)
{
  if (!cs || !variable || !vdef || !value)
    return false;

  for (unsigned int i = 0; i < mutt_array_size(quad_values); i++)
  {
    if (mutt_strcasecmp(quad_values[i], value) == 0)
    {
      *(short *) variable = i;
      return true;
    }
  }

  mutt_buffer_printf(err, "Invalid quad value: %s", value);
  return false;
}

static bool get_quad(void *variable, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!variable || !vdef)
    return false;

  unsigned int index = *(short *) variable;
  if ((index < 0) || (index >= mutt_array_size(quad_values)))
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, quad_values[index]);
  return true;
}

static bool reset_quad(struct ConfigSet *cs, void *variable, const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !variable || !vdef)
    return false;

  *(short *) variable = vdef->initial;
  return true;
}


void init_quad(void)
{
  const struct ConfigSetType cst_quad = { "quad", set_quad, get_quad, reset_quad, NULL };
  cs_register_type(DT_QUAD, &cst_quad);
}
