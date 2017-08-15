#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

const char *magic_values[] = { NULL, "mbox", "MMDF", "MH", "Maildir" };

static bool set_magic(struct ConfigSet *cs, void *var, const struct VariableDef *vdef,
                      const char *value, struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return false;

  int num = -1;
  for (unsigned int i = 0; i < mutt_array_size(magic_values); i++)
  {
    if (mutt_strcasecmp(magic_values[i], value) == 0)
    {
      num = i;
      break;
    }
  }

  if (num < 0)
  {
    mutt_buffer_printf(err, "Invalid magic value: %s", value);
    return false;
  }

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) value, err))
    return false;

  *(short *) var = num;
  return true;
}

static bool get_magic(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false;

  unsigned int index = *(short *) var;
  if ((index < 1) || (index >= mutt_array_size(magic_values)))
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, magic_values[index]);
  return true;
}

static bool reset_magic(struct ConfigSet *cs, void *var,
                        const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  *(short *) var = vdef->initial;
  return true;
}

void init_magic(void)
{
  const struct ConfigSetType cst_magic = { "magic", set_magic, get_magic, reset_magic, NULL };
  cs_register_type(DT_MAGIC, &cst_magic);
}
