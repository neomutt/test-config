#include "config.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "set.h"
#include "types.h"

const char *quad_values[] = { "no", "yes", "ask-no", "ask-yes" };

static bool set_quad(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef,
                     const char *value, struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return false; /* LCOV_EXCL_LINE */

  int num = -1;
  for (unsigned int i = 0; i < mutt_array_size(quad_values); i++)
  {
    if (mutt_strcasecmp(quad_values[i], value) == 0)
    {
      num = i;
      break;
    }
  }

  if (num < 0)
  {
    mutt_buffer_printf(err, "Invalid quad value: %s", value);
    return false;
  }

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) num, err))
    return false;

  *(short *) var = num;
  return true;
}

static bool get_quad(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  unsigned int index = *(short *) var;
  if (index >= mutt_array_size(quad_values))
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, quad_values[index]);
  return true;
}

static bool set_native_quad(const struct ConfigSet *cs, void *var,
                            const struct VariableDef *vdef, intptr_t value,
                            struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  if ((value < 0) || (value >= mutt_array_size(quad_values)))
  {
    mutt_buffer_printf(err, "Invalid quad value: %ld", value);
    return false;
  }

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  *(short *) var = value;
  return true;
}

static intptr_t get_native_quad(const struct ConfigSet *cs, void *var,
                                const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  return *(short *) var;
}

static bool reset_quad(const struct ConfigSet *cs, void *var,
                       const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  *(short *) var = vdef->initial;
  return true;
}

void quad_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_quad = {
    "quad",          set_quad,   get_quad, set_native_quad,
    get_native_quad, reset_quad, NULL,
  };
  cs_register_type(cs, DT_QUAD, &cst_quad);
}
