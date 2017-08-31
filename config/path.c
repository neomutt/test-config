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

static void path_destroy(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef)
{
  if (!cs || !var || !vdef)
    return; /* LCOV_EXCL_LINE */

  /* Don't free strings from the var definition */
  if (*(char **) var == (char *) vdef->initial)
    return;

  FREE(var);
}

static int path_string_set(const struct ConfigSet *cs, void *var,
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

    if (CSR_RESULT(rv) != CSR_SUCCESS)
      return rv | CSR_INV_VALIDATOR;
  }

  path_destroy(cs, var, vdef);

  const char *str = safe_strdup(value);
  int result = CSR_SUCCESS;
  if (!str)
    result |= CSR_SUC_EMPTY;

  *(const char **) var = str;
  return result;
}

static int path_string_get(const struct ConfigSet *cs, void *var,
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

static int path_native_set(const struct ConfigSet *cs, void *var,
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

    if (CSR_RESULT(rv) != CSR_SUCCESS)
      return rv | CSR_INV_VALIDATOR;
  }

  path_destroy(cs, var, vdef);

  str = safe_strdup((const char *) value);
  int result = CSR_SUCCESS;
  if (!str)
    result |= CSR_SUC_EMPTY;

  *(const char **) var = str;
  return result;
}

static intptr_t path_native_get(const struct ConfigSet *cs, void *var,
                                const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  const char *str = *(const char **) var;

  return (intptr_t) str;
}

static int path_reset(const struct ConfigSet *cs, void *var,
                      const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  path_destroy(cs, var, vdef);

  const char *path = (const char *) vdef->initial;

  int result = CSR_SUCCESS;
  if (!path)
    result |= CSR_SUC_EMPTY;

  *(const char **) var = path;
  return result;
}

void path_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_path = {
    "path",          path_string_set, path_string_get, path_native_set,
    path_native_get, path_reset,      path_destroy,
  };
  cs_register_type(cs, DT_PATH, &cst_path);
}
