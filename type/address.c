#include "address.h"
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void destroy_addr(void **obj, struct VariableDef *vdef)
{
  if (!obj || !*obj)
    return;

  struct Address **a = (struct Address **) obj;
  if ((*a)->personal != (const char *) vdef->initial)
    FREE(&(*a)->personal);
  FREE(&(*a)->mailbox);
  FREE(a);
}

static bool set_addr(struct ConfigSet *cs, void *variable, struct VariableDef *vdef,
                     const char *value, struct Buffer *err)
{
  if (!cs || !variable || !vdef || !value)
    return false;

  destroy_addr(variable, vdef);

  struct Address *a = safe_calloc(1, sizeof(*a));

  a->personal = safe_strdup((const char*) value);
  a->mailbox = safe_strdup("dummy1");
  
  *(struct Address **) variable = a;
  return true;
}

static bool get_addr(void *variable, struct VariableDef *vdef, struct Buffer *result)
{
  if (!variable || !vdef)
    return false;

  struct Address *a = *(struct Address **) variable;
  if (!a)
    return true; /* empty string */

  mutt_buffer_addstr(result, a->personal);
  return true;
}

static bool reset_addr(struct ConfigSet *cs, void *variable, struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !variable || !vdef)
    return false;

  destroy_addr(variable, vdef);

  struct Address *a = safe_calloc(1, sizeof(*a));

  a->personal = (char *) vdef->initial;
  a->mailbox = safe_strdup("dummy2");
  
  *(struct Address **) variable = a;
  return true;
}


void init_addr(void)
{
  struct ConfigSetType cst_addr = { "address", set_addr, get_addr, reset_addr, destroy_addr };
  cs_register_type(DT_ADDR, &cst_addr);
}
