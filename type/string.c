#include <stdbool.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void destroy_str(void **obj)
{
  if (!obj || !*obj)
    return;

  FREE(obj);
}

static bool set_str(struct ConfigSet *set, struct HashElem *e,
                    const char *value, struct Buffer *err)
{
  if (e && DTYPE(e->type) != DT_STR)
  {
    mutt_buffer_printf(err, "Variable is not a string");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  if (v->validator && !v->validator(set, v->name, v->type, (intptr_t) value, err))
    return false;

  mutt_str_replace(v->variable, value);
  return true;
}

static bool get_str(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_STR)
  {
    mutt_buffer_printf(result, "Variable is not a string");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  mutt_buffer_addstr(result, *(const char **) v->variable);
  return true;
}


void init_string(void)
{
  struct ConfigSetType cst_str = { "string", set_str, get_str, destroy_str };
  cs_register_type(DT_STR, &cst_str);
}
