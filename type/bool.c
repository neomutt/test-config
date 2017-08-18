#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"
#include "account.h"

const char *bool_values[] = {
  "no", "yes", "false", "true", "0", "1", "off", "on",
};

static bool set_bool(struct ConfigSet *cs, void *var, const struct VariableDef *vdef,
                     const char *value, struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return false;

  int num = -1;
  for (unsigned int i = 0; i < mutt_array_size(bool_values); i++)
  {
    if (mutt_strcasecmp(bool_values[i], value) == 0)
    {
      num = i % 2;
      break;
    }
  }

  if (num < 0)
  {
    mutt_buffer_printf(err, "Invalid boolean value: %s", value);
    return false;
  }

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) num, err))
    return false;

  *(bool *) var = num;
  return true;
}

static bool get_bool(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false;

  unsigned int index = *(bool *) var;
  if (index > 1)
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, bool_values[index]);
  return true;
}

static bool set_native_bool(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  if ((value < 0) || (value > 1))
  {
    mutt_buffer_printf(err, "Invalid boolean value: %s", value);
    return false;
  }

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  *(bool *) var = value;
  return true;
}

static intptr_t get_native_bool(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  return *(bool *) var;
}

static bool reset_bool(struct ConfigSet *cs, void *var,
                       const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  *(bool *) var = vdef->initial;
  return true;
}

void init_bool(struct ConfigSet *cs)
{
  struct ConfigSetType cst_bool = { "boolean", set_bool, get_bool, set_native_bool, get_native_bool, reset_bool, NULL, };
  cs_register_type(cs, DT_BOOL, &cst_bool);
}

