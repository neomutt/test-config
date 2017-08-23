#include <stddef.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "config_set.h"
#include "lib/buffer.h"
#include "lib/string2.h"
#include "mutt_options.h"

static bool set_num(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef,
                    const char *value, struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return false;

  int num = 0;
  if (mutt_atoi(value, &num) < 0)
  {
    mutt_buffer_printf(err, "Invalid number: %s", value);
    return false;
  }

  if ((num < SHRT_MIN) || (num > SHRT_MAX))
  {
    mutt_buffer_printf(err, "Number is too big: %s", value);
    return false;
  }

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) num, err))
    return false;

  *(short *) var = num;
  return true;
}

static bool get_num(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false;

  mutt_buffer_printf(result, "%d", *(short *) var);
  return true;
}

static bool set_native_num(const struct ConfigSet *cs, void *var,
                           const struct VariableDef *vdef, intptr_t value,
                           struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  if ((value < SHRT_MIN) || (value > SHRT_MAX))
  {
    mutt_buffer_printf(err, "Invalid number: %ld", value);
    return false;
  }

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  *(short *) var = value;
  return true;
}

static intptr_t get_native_num(const struct ConfigSet *cs, void *var,
                               const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  return *(short *) var;
}

static bool reset_num(const struct ConfigSet *cs, void *var,
                      const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  *(short *) var = vdef->initial;
  return true;
}

void init_num(struct ConfigSet *cs)
{
  struct ConfigSetType cst_num = {
    "number", set_num, get_num, set_native_num, get_native_num, reset_num, NULL,
  };
  cs_register_type(cs, DT_NUM, &cst_num);
}
