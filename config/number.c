#include "config.h"
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "lib/buffer.h"
#include "lib/string2.h"
#include "set.h"
#include "types.h"

static int number_string_set(const struct ConfigSet *cs, void *var,
                             const struct VariableDef *vdef, const char *value,
                             struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  int num = 0;
  if (mutt_atoi(value, &num) < 0)
  {
    mutt_buffer_printf(err, "Invalid number: %s", value);
    return CSR_ERR_INVALID | CSR_INV_TYPE;
  }

  if ((num < SHRT_MIN) || (num > SHRT_MAX))
  {
    mutt_buffer_printf(err, "Number is too big: %s", value);
    return CSR_ERR_INVALID | CSR_INV_TYPE;
  }

  int result = CSR_SUCCESS;
  if (vdef->validator)
    result = vdef->validator(cs, vdef, (intptr_t) num, err);

  if ((result & CSR_RESULT_MASK) != CSR_SUCCESS)
    return result | CSR_INV_VALIDATOR;

  *(short *) var = num;
  return CSR_SUCCESS;
}

static int number_string_get(const struct ConfigSet *cs, void *var,
                             const struct VariableDef *vdef, struct Buffer *result)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  mutt_buffer_printf(result, "%d", *(short *) var);
  return CSR_SUCCESS;
}

static int number_native_set(const struct ConfigSet *cs, void *var,
                             const struct VariableDef *vdef, intptr_t value,
                             struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  if ((value < SHRT_MIN) || (value > SHRT_MAX))
  {
    mutt_buffer_printf(err, "Invalid number: %ld", value);
    return CSR_ERR_INVALID | CSR_INV_TYPE;
  }

  int result = CSR_SUCCESS;
  if (vdef->validator)
    result = vdef->validator(cs, vdef, value, err);

  if ((result & CSR_RESULT_MASK) != CSR_SUCCESS)
    return result | CSR_INV_VALIDATOR;

  *(short *) var = value;
  return CSR_SUCCESS;
}

static intptr_t number_native_get(const struct ConfigSet *cs, void *var,
                                  const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  return *(short *) var;
}

static int number_reset(const struct ConfigSet *cs, void *var,
                        const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  *(short *) var = vdef->initial;
  return CSR_SUCCESS;
}

void number_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_number = {
    "number", number_string_set, number_string_get, number_native_set, number_native_get, number_reset, NULL,
  };
  cs_register_type(cs, DT_NUMBER, &cst_number);
}
