#include <stdbool.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void destroy_str(void **obj, struct VariableDef *vdef)
{
  if (!obj || !*obj || !vdef)
    return;

  /* Don't free strings from the variable definition */
  if (*obj == (void *) vdef->initial)
    return;

  FREE(obj);
}

static bool set_str(struct ConfigSet *cs, void *variable, struct VariableDef *vdef,
                    const char *value, struct Buffer *err)
{
  if (!cs || !variable || !vdef || !value)
    return false;

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) value, err))
    return false;

  destroy_str(variable, vdef);
  *(const char **) variable = safe_strdup(value);
  return true;
}

static bool get_str(void *variable, struct VariableDef *vdef, struct Buffer *result)
{
  if (!variable || !vdef)
    return false;

  const char *str = *(const char **) variable;
  if (!str)
    return true; /* empty string */

  mutt_buffer_addstr(result, str);
  return true;
}

static bool reset_str(struct ConfigSet *cs, void *variable,
                      struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !variable || !vdef)
    return false;

  destroy_str(variable, vdef);
  *(const char **) variable = (const char *) vdef->initial;
  return true;
}

void init_string(void)
{
  const struct ConfigSetType cst_str = { "string", set_str, get_str, reset_str, destroy_str };
  cs_register_type(DT_STR, &cst_str);
}
