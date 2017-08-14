#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

const char *quad_values[] = { "no", "yes", "ask-no", "ask-yes" };

static bool set_quad(struct ConfigSet *set, void *variable, struct VariableDef *def,
                     const char *value, struct Buffer *err)
{
  // if (DTYPE(e->type) != DT_QUAD)
  // {
  //   mutt_buffer_printf(err, "Variable is not a quad");
  //   return false;
  // }

  // struct VariableDef *v = e->data;
  // if (!v)
  //   return false;

  for (unsigned int i = 0; i < mutt_array_size(quad_values); i++)
  {
    if (mutt_strcasecmp(quad_values[i], value) == 0)
    {
      // *(short *) v->variable = i;
      *(short *) variable = i;
      return true;
    }
  }

  mutt_buffer_printf(err, "Invalid quad value: %s", value);
  return false;
}

static bool get_quad(struct HashElem *e, struct Buffer *result)
{
  // if (DTYPE(e->type) != DT_QUAD)
  // {
  //   mutt_buffer_printf(result, "Variable is not a quad");
  //   return false;
  // }

  // struct VariableDef *v = e->data;
  // if (!v)
  //   return false;

  void *variable = e;

  // unsigned int index = *(short *) v->variable;
  unsigned int index = *(short *) variable;
  if (index >= mutt_array_size(quad_values))
  {
    mutt_buffer_printf(result, "Variable has an invalid value");
    return false;
  }

  mutt_buffer_addstr(result, quad_values[index]);
  return true;
}

static bool reset_quad(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_QUAD)
  {
    mutt_buffer_printf(err, "Variable is not a quad");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  *(short *) v->variable = v->initial;
  return true;
}


void init_quad(void)
{
  struct ConfigSetType cst_quad = { "quad", set_quad, get_quad, reset_quad, NULL };
  cs_register_type(DT_QUAD, &cst_quad);
}
