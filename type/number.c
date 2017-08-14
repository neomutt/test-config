#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static bool set_num(struct ConfigSet *cs, void *variable, const struct VariableDef *vdef,
                    const char *value, struct Buffer *err)
{
  if (!cs || !variable || !vdef || !value)
    return false;

  int num = 0;
  if (mutt_atoi(value, &num) < 0)
  {
    mutt_buffer_printf(err, "Invalid number: %s", value);
    return false;
  }

  if (num > SHRT_MAX)
    return false;

  *(short *) variable = num;
  return true;
}

static bool get_num(void *variable, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!variable || !vdef)
    return false;

  mutt_buffer_printf(result, "%d", *(short*) variable);
  return true;
}

static bool reset_num(struct ConfigSet *cs, void *variable, const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !variable || !vdef)
    return false;

  *(short *) variable = vdef->initial;
  return true;
}


void init_num(void)
{
  const struct ConfigSetType cst_num = { "number", set_num, get_num, reset_num, NULL };
  cs_register_type(DT_NUM, &cst_num);
}
