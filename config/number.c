#include "config.h"
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "lib/buffer.h"
#include "lib/string2.h"
#include "set.h"
#include "types.h"

static bool set_number(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef,
                       const char *value, struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return false; /* LCOV_EXCL_LINE */

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

static bool get_number(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  mutt_buffer_printf(result, "%d", *(short *) var);
  return true;
}

static bool set_native_number(const struct ConfigSet *cs, void *var,
                              const struct VariableDef *vdef, intptr_t value,
                              struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

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

static intptr_t get_native_number(const struct ConfigSet *cs, void *var,
                                  const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  return *(short *) var;
}

static bool reset_number(const struct ConfigSet *cs, void *var,
                         const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  *(short *) var = vdef->initial;
  return true;
}

void number_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_number = {
    "number",          set_number,   get_number, set_native_number,
    get_native_number, reset_number, NULL,
  };
  cs_register_type(cs, DT_NUMBER, &cst_number);
}
