#include <stdbool.h>
#include <string.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void destroy_str(void **obj, struct VariableDef *def)
{
  if (!obj || !*obj || !def)
    return;

  /* Don't free strings from the variable definition */
  if (*obj == (void *) def->initial)
    return;

  FREE(obj);
}

static bool set_str(struct ConfigSet *set, void *variable,
                    struct VariableDef *def, const char *value, struct Buffer *err)
{
  if (!set || !variable || !def || !value)
    return false;

  if (def->validator && !def->validator(set, def, (intptr_t) value, err))
    return false;

  destroy_str(variable, def);
  *(const char **) variable = safe_strdup(value);
  return true;
}

static bool get_str(void *variable, struct VariableDef *def, struct Buffer *result)
{
  if (!variable || !def)
    return false;

  const char *str = *(const char **) variable;
  if (!str)
    return true; /* empty string */

  mutt_buffer_addstr(result, str);
  return true;
}

static bool reset_str(struct ConfigSet *set, void *variable,
                      struct VariableDef *def, struct Buffer *err)
{
  if (!set || !variable || !def)
    return false;

  destroy_str(variable, def);
  *(const char **) variable = (const char *) def->initial; 
  return true;
}

void init_string(void)
{
  struct ConfigSetType cst_str = { "string", set_str, get_str, reset_str, destroy_str };
  cs_register_type(DT_STR, &cst_str);
}
