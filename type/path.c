#include <stdbool.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void destroy_path(void **obj, const struct VariableDef *vdef)
{
  if (!obj || !*obj || !vdef)
    return;

  /* Don't free strings from the var definition */
  if (*obj == (void *) vdef->initial)
    return;

  FREE(obj);
}

static bool set_path(struct ConfigSet *cs, void *var, const struct VariableDef *vdef,
                     const char *value, struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return false;

  destroy_path(var, vdef);
  *(const char **) var = safe_strdup(value);
  return true;
}

static bool get_path(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false;

  const char *str = *(const char **) var;
  if (!str)
    return true; /* empty string */

  mutt_buffer_addstr(result, str);
  return true;
}

static bool reset_path(struct ConfigSet *cs, void *var,
                       const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  destroy_path(var, vdef);
  *(const char **) var = (const char *) vdef->initial;
  return true;
}

void init_path(void)
{
  const struct ConfigSetType cst_path = { "path", set_path, get_path, reset_path, destroy_path };
  cs_register_type(DT_PATH, &cst_path);
}
