#include <stdbool.h>
#include "buffer.h"
#include "config_set.h"
#include "hash.h"
#include "lib.h"
#include "mutt_options.h"

const char *bool_values[] = {
  "no",
  "yes",
  "false",
  "true",
  "0",
  "1",
  "off",
  "on",
};

static bool set_bool(struct ConfigSet *set, struct HashElem *e,
                     const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_BOOL)
  {
    mutt_buffer_printf(err, "Variable is not a boolean");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  for (intptr_t i = 0; i < mutt_array_size(bool_values); i++)
  {
    if (mutt_strcasecmp(bool_values[i], value) == 0)
    {
      *(bool *) v->variable = i % 2;
      return true;
    }
  }

  mutt_buffer_printf(err, "Invalid boolean value: %s", value);
  return false;
}

static bool get_bool(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_BOOL)
  {
    mutt_buffer_printf(result, "Variable is not an boolean");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  int index = *(bool *) v->variable;
  if ((index < 0) || (index > mutt_array_size(bool_values)))
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, bool_values[index]);
  return true;
}

static bool reset_bool(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_BOOL)
  {
    mutt_buffer_printf(err, "Variable is not an boolean");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  *(bool *) v->variable = v->initial;
  return true;
}


void init_bool(void)
{
  struct ConfigSetType cst_bool = { "boolean", set_bool, get_bool, reset_bool, NULL };
  cs_register_type(DT_BOOL, &cst_bool);
}
