#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

const char *bool_values[] = {
  "no", "yes", "false", "true", "0", "1", "off", "on",
};

static bool set_bool(struct ConfigSet *cs, void *variable,
                     struct VariableDef *vdef, const char *value, struct Buffer *err)
{
  if (!cs || !variable || !vdef || !value)
    return false;

  for (unsigned int i = 0; i < mutt_array_size(bool_values); i++)
  {
    if (mutt_strcasecmp(bool_values[i], value) == 0)
    {
      *(bool *) variable = i % 2;
      return true;
    }
  }

  mutt_buffer_printf(err, "Invalid boolean value: %s", value);
  return false;
}

static bool get_bool(void *variable, struct VariableDef *vdef, struct Buffer *result)
{
  if (!variable || !vdef)
    return false;

  if (!variable || !vdef)
    return false;

  unsigned int index = *(bool *) variable;
  if (index > 1)
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, bool_values[index]);
  return true;
}

static bool reset_bool(struct ConfigSet *cs, void *variable,
                       struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !variable || !vdef)
    return false;

  *(bool *) variable = vdef->initial;
  return true;
}

void init_bool(void)
{
  struct ConfigSetType cst_bool = { "boolean", set_bool, get_bool, reset_bool, NULL };
  cs_register_type(DT_BOOL, &cst_bool);
}
