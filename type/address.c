#include "address.h"
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void destroy_addr(void **obj)
{
  if (!obj || !*obj)
    return;

  struct Address **a = (struct Address **) obj;
  FREE((*a)->personal);
  FREE((*a)->mailbox);
  FREE(a);
}

static bool set_addr(struct ConfigSet *set, struct HashElem *e,
                     const char *value, struct Buffer *err)
{
  if (DTYPE(e->type) != DT_ADDR)
  {
    mutt_buffer_printf(err, "Variable is not an address");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  struct Address *a = safe_calloc(1, sizeof(*a));

  destroy_addr(v->variable);

  a->personal = safe_strdup((const char*) value);
  
  *(struct Address **) v->variable = a;
  return true;
}

static bool get_addr(struct HashElem *e, struct Buffer *result)
{
  if (DTYPE(e->type) != DT_ADDR)
  {
    mutt_buffer_printf(result, "Variable is not an address");
    return false;
  }

  struct VariableDef *v = e->data;
  if (!v)
    return false;

  struct Address *a = *(struct Address **) v->variable;
  if (!a)
    return false;

  mutt_buffer_addstr(result, a->personal);
  return true;
}


void init_addr(void)
{
  struct ConfigSetType cst_addr = { "address", set_addr, get_addr, destroy_addr };
  cs_register_type(DT_ADDR, &cst_addr);
}
