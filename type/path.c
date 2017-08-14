#include <stdbool.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void destroy_path(void **obj, struct VariableDef *def)
{
  if (!obj || !*obj)
    return;

  //XXX FREE(obj);
}

static bool set_path(struct ConfigSet *set, void *variable, struct VariableDef *def,
                     const char *value, struct Buffer *err)
{
  *(const char **) variable = safe_strdup(value);
  return true;
}

static bool get_path(void *variable, struct VariableDef *def, struct Buffer *result)
{
  // return true; /* empty string */

  mutt_buffer_addstr(result, *(const char **) variable);
  return true;
}

static bool reset_path(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_PATH)
  {
    mutt_buffer_printf(err, "Variable is not a path");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  mutt_str_replace(v->variable, (const char*) v->initial);
  return true;
}


void init_path(void)
{
  struct ConfigSetType cst_path = { "path", set_path, get_path, reset_path, destroy_path };
  cs_register_type(DT_PATH, &cst_path);
}
