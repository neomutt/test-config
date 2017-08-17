#include "address.h"
#include "config_set.h"
#include "lib/lib.h"
#include "mutt_options.h"

static void destroy_addr(void *var, const struct VariableDef *vdef)
{
  if (!var || !vdef)
    return;

  struct Address **a = (struct Address **) var;
  if (!*a)
    return;

  if ((*a)->personal != (char *) vdef->initial)
    FREE(&(*a)->personal);

  FREE(&(*a)->mailbox);
  FREE(a);
}

static bool set_addr(struct ConfigSet *cs, void *var, const struct VariableDef *vdef,
                     const char *value, struct Buffer *err)
{
  if (!cs || !var || !vdef || !value)
    return false;

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) value, err))
    return false;

  destroy_addr(var, vdef);

  struct Address *a = safe_calloc(1, sizeof(*a));
  a->personal = safe_strdup((const char *) value);
  a->mailbox = safe_strdup("dummy1");

  *(struct Address **) var = a;
  return true;
}

static bool get_addr(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false;

  struct Address *a = *(struct Address **) var;
  if (!a)
    return true; /* empty string */

  mutt_buffer_addstr(result, a->personal);
  return true;
}

static bool set_native_addr(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  return false;
}

static intptr_t get_native_addr(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, struct Buffer *err)
{
  return -1;
}

static bool reset_addr(struct ConfigSet *cs, void *var,
                       const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  destroy_addr(var, vdef);

  struct Address *a = safe_calloc(1, sizeof(*a));

  a->personal = (char *) vdef->initial;
  a->mailbox = safe_strdup("dummy2");

  *(struct Address **) var = a;
  return true;
}

void init_addr(void)
{
  const struct ConfigSetType cst_addr = { "address", set_addr, get_addr, set_native_addr, get_native_addr, reset_addr, destroy_addr, };
  cs_register_type(DT_ADDR, &cst_addr);
}
