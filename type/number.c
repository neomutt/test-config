#include <stdbool.h>
#include "buffer.h"
#include "hash.h"
#include "config_set.h"
#include "mutt_options.h"
#include "lib.h"

static bool set_num(struct ConfigSet *set, struct HashElem *e, const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_NUM)
  {
    mutt_buffer_printf(err, "Variable is not a number");
    return false;
  }

  int num = 0;
  if (mutt_atoi(value, &num) < 0)
  {
    mutt_buffer_printf(err, "Invalid number: %s", value);
    return false;
  }

  intptr_t copy = num;
  e->data = (void *) copy;
  return true;
}

static bool get_num(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_NUM)
  {
    mutt_buffer_printf(result, "Variable is not a string");
    return false;
  }

  mutt_buffer_printf(result, "%d", (intptr_t) e->data);
  return true;
}

static bool reset_num(struct ConfigSet *set, struct HashElem *e, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_NUM)
  {
    mutt_buffer_printf(err, "Variable is not a number");
    return false;
  }

  return false;
}


void init_num(void)
{
  struct ConfigSetType cst_num = { set_num, get_num, reset_num, NULL };
  cs_register_type("number", DT_NUM, &cst_num);
}
