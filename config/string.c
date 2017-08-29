#include "config.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "set.h"
#include "types.h"

static void string_destroy(void *var, const struct VariableDef *vdef)
{
  if (!var || !vdef)
    return; /* LCOV_EXCL_LINE */

  const char **str = (const char **) var;
  if (!*str)
    return;

  /* Don't free strings from the var definition */
  if (*(char **) var == (char *) vdef->initial)
    return;

  FREE(var);
}

static bool string_string_set(const struct ConfigSet *cs, void *var,
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

  string_destroy(var, vdef);

  *(const char **) var = safe_strdup(value);
  return true;
}

static bool string_string_get(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  const char *str = *(const char **) var;
  if (!str)
    return true; /* empty string */

  mutt_buffer_addstr(result, str);
  return true;
}

static bool string_native_set(const struct ConfigSet *cs, void *var,
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

  /* Don't free strings from the var definition */
  if (*(char **) var != (char *) vdef->initial)
    FREE(var);

  *(const char **) var = safe_strdup(str);
  return true;
}

static intptr_t string_native_get(const struct ConfigSet *cs, void *var,
                                  const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  const char *str = *(const char **) var;

  return (intptr_t) str;
}

static bool string_reset(const struct ConfigSet *cs, void *var,
                         const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  string_destroy(var, vdef);

  *(const char **) var = (const char *) vdef->initial;
  return true;
}

void string_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_string = {
    "string",          string_string_set, string_string_get, string_native_set,
    string_native_get, string_reset,      string_destroy,
  };
  cs_register_type(cs, DT_STRING, &cst_string);
}
