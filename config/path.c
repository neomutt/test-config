#include "config.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "set.h"
#include "types.h"

static void path_destroy(void *var, const struct VariableDef *vdef)
{
  if (!var || !vdef)
    return; /* LCOV_EXCL_LINE */

  /* Don't free strings from the var definition */
  if (*(char **) var == (char *) vdef->initial)
    return;

  FREE(var);
}

static bool path_string_set(const struct ConfigSet *cs, void *var,
                            const struct VariableDef *vdef, const char *value,
                            struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  /* Store empty strings as NULL */
  if (value && (value[0] == '\0'))
    value = NULL;

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) value, err))
    return false;

  path_destroy(var, vdef);

  *(const char **) var = safe_strdup(value);
  return true;
}

static bool path_string_get(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  const char *str = *(const char **) var;
  if (!str)
    return true; /* empty string */

  mutt_buffer_addstr(result, str);
  return true;
}

static bool path_native_set(const struct ConfigSet *cs, void *var,
                            const struct VariableDef *vdef, intptr_t value,
                            struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  const char *str = (const char *) value;

  /* Store empty strings as NULL */
  if (str && (str[0] == '\0'))
    value = 0;

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  path_destroy(var, vdef);

  *(const char **) var = safe_strdup((const char *) value);
  return true;
}

static intptr_t path_native_get(const struct ConfigSet *cs, void *var,
                                const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  const char *str = *(const char **) var;

  return (intptr_t) str;
}

static bool path_reset(const struct ConfigSet *cs, void *var,
                       const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  path_destroy(var, vdef);

  *(const char **) var = (const char *) vdef->initial;
  return true;
}

void path_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_path = {
    "path",          path_string_set, path_string_get, path_native_set,
    path_native_get, path_reset,      path_destroy,
  };
  cs_register_type(cs, DT_PATH, &cst_path);
}
