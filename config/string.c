#include "config.h"
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "set.h"
#include "types.h"

static void string_destroy(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef)
{
  if (!cs || !var || !vdef)
    return; /* LCOV_EXCL_LINE */

  const char **str = (const char **) var;
  if (!*str)
    return;

  /* Don't free strings from the var definition */
  if (*(char **) var == (char *) vdef->initial)
    return;

  FREE(var);
}

static int string_string_set(const struct ConfigSet *cs, void *var,
                             const struct VariableDef *vdef, const char *value,
                             struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  /* Store empty strings as NULL */
  if (value && (value[0] == '\0'))
    value = NULL;

  if (vdef->validator)
  {
    int rv = vdef->validator(cs, vdef, (intptr_t) value, err);

    if ((rv & CSR_RESULT_MASK) != CSR_SUCCESS)
      return rv | CSR_INV_VALIDATOR;
  }

  string_destroy(cs, var, vdef);

  const char *str = safe_strdup(value);
  int result = CSR_SUCCESS;
  if (!str)
    result |= CSR_SUC_EMPTY;

  *(const char **) var = str;
  return result;
}

static int string_string_get(const struct ConfigSet *cs, void *var,
                             const struct VariableDef *vdef, struct Buffer *result)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  const char *str = *(const char **) var;
  if (!str)
    return CSR_SUCCESS | CSR_SUC_EMPTY; /* empty string */

  mutt_buffer_addstr(result, str);
  return CSR_SUCCESS;
}

static int string_native_set(const struct ConfigSet *cs, void *var,
                             const struct VariableDef *vdef, intptr_t value,
                             struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  const char *str = (const char *) value;

  /* Store empty strings as NULL */
  if (str && (str[0] == '\0'))
    value = 0;

  if (vdef->validator)
  {
    int rv = vdef->validator(cs, vdef, value, err);

    if ((rv & CSR_RESULT_MASK) != CSR_SUCCESS)
      return rv | CSR_INV_VALIDATOR;
  }

  /* Don't free strings from the var definition */
  if (*(char **) var != (char *) vdef->initial)
    FREE(var);

  str = safe_strdup(str);
  int result = CSR_SUCCESS;
  if (!str)
    result |= CSR_SUC_EMPTY;

  *(const char **) var = str;
  return result;
}

static intptr_t string_native_get(const struct ConfigSet *cs, void *var,
                                  const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  const char *str = *(const char **) var;

  return (intptr_t) str;
}

static int string_reset(const struct ConfigSet *cs, void *var,
                        const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  string_destroy(cs, var, vdef);

  const char *str = (const char *) vdef->initial;
  int result = CSR_SUCCESS;
  if (!str)
    result |= CSR_SUC_EMPTY;

  *(const char **) var = str;
  return result;
}

void string_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_string = {
    "string",          string_string_set, string_string_get, string_native_set,
    string_native_get, string_reset,      string_destroy,
  };
  cs_register_type(cs, DT_STRING, &cst_string);
}
