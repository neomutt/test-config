#include <stdbool.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void destroy_str(void *var, const struct VariableDef *vdef)
{
  if (!var || !vdef)
    return;

  const char **str = (const char **) var;
  if (!*str)
    return;

  /* Don't free strings from the var definition */
  if (*(char **) var == (char *) vdef->initial)
    return;

  FREE(var);
}

static bool set_str(struct ConfigSet *cs, void *var, const struct VariableDef *vdef,
                    const char *value, struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return false;

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) value, err))
    return false;

  destroy_str(var, vdef);

  *(const char **) var = safe_strdup(value);
  return true;
}

static bool get_str(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false;

  const char *str = *(const char **) var;
  if (!str)
    return true; /* empty string */

  mutt_buffer_addstr(result, str);
  return true;
}

static bool set_native_str(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  FREE(var);

  const char *str = safe_strdup((const char *) value);

  *(const char **) var = str;
  return true;
}

static intptr_t get_native_str(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  const char *str = *(const char **) var;

  return (intptr_t) str;
}

static bool reset_str(struct ConfigSet *cs, void *var,
                      const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  destroy_str(var, vdef);

  *(const char **) var = (const char *) vdef->initial;
  return true;
}

void init_string(struct ConfigSet *cs)
{
  struct ConfigSetType cst_str = { "string", set_str, get_str, set_native_str, get_native_str, reset_str, destroy_str, };
  cs_register_type(cs, DT_STR, &cst_str);
}
