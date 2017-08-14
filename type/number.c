#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static bool set_num(struct ConfigSet *set, void *variable, struct VariableDef *def,
                    const char *value, struct Buffer *err)
{
  int num = 0;
  if (mutt_atoi(value, &num) < 0)
  {
    mutt_buffer_printf(err, "Invalid number: %s", value);
    return false;
  }

  if (num > SHRT_MAX)
    return false;

  *(short *) variable = num;
  return true;
}

static bool get_num(void *variable, struct VariableDef *def, struct Buffer *result)
{
  mutt_buffer_printf(result, "%d", *(short*) variable);
  return true;
}

static bool reset_num(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_NUM)
  {
    mutt_buffer_printf(err, "Variable is not a number");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  *(short *) v->variable = v->initial;
  return true;
}


void init_num(void)
{
  struct ConfigSetType cst_num = { "number", set_num, get_num, reset_num, NULL };
  cs_register_type(DT_NUM, &cst_num);
}
