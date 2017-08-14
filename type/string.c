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

  if (*obj == (void*) def->initial)
  {
    // printf("ORIG STRING: '%s'\n", (const char*) *obj);
  }
  else
  {
    FREE(obj);
  }
}

static bool set_str(struct ConfigSet *set, void *variable, struct VariableDef *def,
                    const char *value, struct Buffer *err)
{
  // if (e && DTYPE(e->type) != DT_STR)
  // {
  //   mutt_buffer_printf(err, "Variable is not a string");
  //   return false;
  // }

  // struct VariableDef *def = e->data;
  // if (!def)
  //   return false;

  //XXX if (def->validator && !def->validator(set, def->name, def->type, (intptr_t) value, err))
  //XXX   return false;

  destroy_str(variable, def);
  // mutt_str_replace(def->variable, value);
  *(const char **) variable = safe_strdup(value);
  return true;
}

static bool get_str(void *variable, struct VariableDef *def, struct Buffer *result)
{
  // if (DTYPE(e->type) != DT_STR)
  // {
  //   mutt_buffer_printf(result, "Variable is not a string");
  //   return false;
  // }

  // struct VariableDef *v = e->data;
  // if (!v)
  //   return false;

  // return true; /* empty string */

  // mutt_buffer_addstr(result, *(const char **) v->variable);
  mutt_buffer_addstr(result, *(const char **) variable);
  return true;
}

static bool reset_str(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (e && DTYPE(e->type) != DT_STR)
  {
    mutt_buffer_printf(err, "Variable is not a string");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  mutt_str_replace(v->variable, (const char*) v->initial);
  return true;
}


void init_string(void)
{
  struct ConfigSetType cst_str = { "string", set_str, get_str, reset_str, destroy_str };
  cs_register_type(DT_STR, &cst_str);
}
