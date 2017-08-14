#include <stdbool.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void destroy_path(void **obj, struct VariableDef *vdef)
{
  if (!obj || !*obj)
    return;

  //XXX FREE(obj);
}

static bool set_path(struct ConfigSet *cs, void *variable, struct VariableDef *vdef,
                     const char *value, struct Buffer *err)
{
  if (!cs || !variable || !vdef || !value)
    return false;

  *(const char **) variable = safe_strdup(value);
  return true;
}

static bool get_path(void *variable, struct VariableDef *vdef, struct Buffer *result)
{
  if (!variable || !vdef)
    return false;

  // return true; /* empty string */

  mutt_buffer_addstr(result, *(const char **) variable);
  return true;
}

static bool reset_path(struct ConfigSet *cs, void *variable, struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !variable || !vdef)
    return false;

  mutt_str_replace(variable, (const char*) vdef->initial);
  return true;
}


void init_path(void)
{
  struct ConfigSetType cst_path = { "path", set_path, get_path, reset_path, destroy_path };
  cs_register_type(DT_PATH, &cst_path);
}
