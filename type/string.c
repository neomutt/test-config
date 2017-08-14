#include <stdbool.h>
#include <string.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void destroy_str(void **obj, struct VariableDef *def)
{
  if (!obj || !*obj)
    return;

  // printf("obj: '%s'\n", (const char*) *obj);
  // printf("def: '%s'\n", (const char*) def->initial);

  if (*obj != (void*) def->initial)
    FREE(obj);
}

static bool set_str(struct ConfigSet *set, void *variable, struct VariableDef *def,
                    const char *value, struct Buffer *err)
{
  //XXX if (def->validator && !def->validator(set, def->name, def->type, (intptr_t) value, err))
  //XXX   return false;

  destroy_str(variable, def);
  // mutt_str_replace(def->variable, value);
  *(const char **) variable = safe_strdup(value);
  return true;
}

static bool get_str(void *variable, struct VariableDef *def, struct Buffer *result)
{
  // return true; /* empty string */

  mutt_buffer_addstr(result, *(const char **) variable);
  return true;
}

static bool reset_str(struct ConfigSet *set, void *variable, struct VariableDef *def, struct Buffer *err)
{
  mutt_str_replace(variable, (const char*) def->initial);
  return true;
}


void init_string(void)
{
  struct ConfigSetType cst_str = { "string", set_str, get_str, reset_str, destroy_str };
  cs_register_type(DT_STR, &cst_str);
}
