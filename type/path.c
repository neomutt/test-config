#include <stdbool.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void destroy_path(void **obj)
{
  if (!obj || !*obj)
    return;

  FREE(obj);
}

static bool set_path(struct ConfigSet *set, struct HashElem *e,
                     const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_PATH)
  {
    mutt_buffer_printf(err, "Variable is not a path");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  mutt_str_replace(v->variable, value);
  return true;
}

static bool get_path(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_PATH)
  {
    mutt_buffer_printf(result, "Variable is not a path");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  mutt_buffer_addstr(result, *(const char **) v->variable);
  return true;
}


void init_path(void)
{
  struct ConfigSetType cst_path = { "path", set_path, get_path, destroy_path };
  cs_register_type(DT_PATH, &cst_path);
}
