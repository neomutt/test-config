#include "config.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "config_set.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "types.h"

static void destroy_path(void *var, const struct VariableDef *vdef)
{
  if (!var || !vdef)
    return; /* LCOV_EXCL_LINE */

  /* Don't free strings from the var definition */
  if (*(char **) var == (char *) vdef->initial)
    return;

  FREE(var);
}

static bool set_path(const struct ConfigSet *cs, void *var, const struct VariableDef *vdef,
                     const char *value, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  /* Store empty strings as NULL */
  if (value && (value[0] == '\0'))
    value = NULL;

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) value, err))
    return false;

  destroy_path(var, vdef);

  *(const char **) var = safe_strdup(value);
  return true;
}

static bool get_path(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  const char *str = *(const char **) var;
  if (!str)
    return true; /* empty string */

  mutt_buffer_addstr(result, str);
  return true;
}

static bool set_native_path(const struct ConfigSet *cs, void *var,
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

  destroy_path(var, vdef);

  *(const char **) var = safe_strdup((const char *) value);
  return true;
}

static intptr_t get_native_path(const struct ConfigSet *cs, void *var,
                                const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  const char *str = *(const char **) var;

  return (intptr_t) str;
}

static bool reset_path(const struct ConfigSet *cs, void *var,
                       const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  destroy_path(var, vdef);

  *(const char **) var = (const char *) vdef->initial;
  return true;
}

void init_path(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_path = {
    "path",          set_path,   get_path,     set_native_path,
    get_native_path, reset_path, destroy_path,
  };
  cs_register_type(cs, DT_PATH, &cst_path);
}
