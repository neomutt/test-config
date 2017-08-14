#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static bool set_num(struct ConfigSet *set, struct HashElem *e,
                    const char *value, struct Buffer *err)
{
  // if (DTYPE(e->type) != DT_NUM)
  // {
  //   mutt_buffer_printf(err, "Variable is not a number");
  //   return false;
  // }

  int num = 0;
  if (mutt_atoi(value, &num) < 0)
  {
    mutt_buffer_printf(err, "Invalid number: %s", value);
    return false;
  }

  if (num > SHRT_MAX)
    return false;

  // struct VariableDef *v = e->data;
  // if (!v)
  //   return false;

  // *(short *) v->variable = num;
  *(short *) e = num;
  return true;
}

static bool get_num(struct HashElem *e, struct Buffer *result)
{
  // if (DTYPE(e->type) != DT_NUM)
  // {
  //   mutt_buffer_printf(result, "Variable is not a string");
  //   return false;
  // }

  // struct VariableDef *v = e->data;
  // if (!v)
  //   return false;

  // mutt_buffer_printf(result, "%d", *(short*) v->variable);
  mutt_buffer_printf(result, "%d", *(short*) e);
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
