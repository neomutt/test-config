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

static struct Address *dup_address(struct Address *addr)
{
  struct Address *a = safe_calloc(1, sizeof(*a));
  a->personal = safe_strdup(addr->personal);
  return a;
}

static bool set_native_addr(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, intptr_t value, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  addr_free(var);

  struct Address *addr = dup_address((struct Address *) value);

  *(struct Address **) var = addr;
  return true;
}

static intptr_t get_native_addr(struct ConfigSet *cs, void *var, const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  struct Address *addr = *(struct Address **) var;

  return (intptr_t) addr;
}

static bool reset_addr(struct ConfigSet *cs, void *var,
                       const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false;

  destroy_addr(var, vdef);

  struct Address *a = safe_calloc(1, sizeof(*a));

  a->personal = safe_strdup((char *) vdef->initial);
  a->mailbox = safe_strdup("dummy2");

  *(struct Address **) var = a;
  return true;
}

void init_addr(struct ConfigSet *cs)
{
  struct ConfigSetType cst_addr = { "address", set_addr, get_addr, set_native_addr, get_native_addr, reset_addr, destroy_addr, };
  cs_register_type(cs, DT_ADDR, &cst_addr);
}

struct Address *addr_create(const char *addr)
{
  struct Address *a = safe_calloc(1, sizeof(*a));
  a->personal = safe_strdup(addr);
  return a;
}

void addr_free(struct Address **addr)
{
  if (!addr || !*addr)
    return;

  FREE(&(*addr)->personal);
  FREE(&(*addr)->mailbox);
  FREE(addr);
}
