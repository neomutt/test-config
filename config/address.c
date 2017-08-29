#include "config.h"
#include <stddef.h>
#include <stdint.h>
#include "address.h"
#include "config_set.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "types.h"

static void destroy_address(void *var, const struct VariableDef *vdef)
{
  if (!var || !vdef)
    return; /* LCOV_EXCL_LINE */

  struct Address **a = (struct Address **) var;
  if (!*a)
    return;

  FREE(&(*a)->personal);
  FREE(&(*a)->mailbox);
  FREE(a);
}

static bool set_address(const struct ConfigSet *cs, void *var,
                        const struct VariableDef *vdef, const char *value,
                        struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  struct Address *addr = NULL;

  // An empty address "" will be stored as NULL
  if (value && (value[0] != '\0'))
  {
    addr = safe_calloc(1, sizeof(*addr));
    addr->personal = safe_strdup((const char *) value);
    addr->mailbox = safe_strdup("dummy1");
  }

  if (vdef->validator && !vdef->validator(cs, vdef, (intptr_t) addr, err))
  {
    destroy_address(&addr, vdef);
    return false;
  }

  destroy_address(var, vdef);

  *(struct Address **) var = addr;
  return true;
}

static bool get_address(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  struct Address *a = *(struct Address **) var;
  if (!a)
    return true; /* empty string */

  mutt_buffer_addstr(result, a->personal);
  return true;
}

static struct Address *dup_address(struct Address *addr)
{
  if (!addr)
    return NULL; /* LCOV_EXCL_LINE */

  struct Address *a = safe_calloc(1, sizeof(*a));
  a->personal = safe_strdup(addr->personal);
  a->mailbox = safe_strdup(addr->mailbox);
  return a;
}

static bool set_native_address(const struct ConfigSet *cs, void *var,
                               const struct VariableDef *vdef, intptr_t value,
                               struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  address_free(var);

  *(struct Address **) var = dup_address((struct Address *) value);
  return true;
}

static intptr_t get_native_address(const struct ConfigSet *cs, void *var,
                                   const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  struct Address *addr = *(struct Address **) var;

  return (intptr_t) addr;
}

struct Address *address_create(const char *addr)
{
  struct Address *a = safe_calloc(1, sizeof(*a));
  a->personal = safe_strdup(addr);
  a->mailbox = safe_strdup("dummy3");
  return a;
}

static bool reset_address(const struct ConfigSet *cs, void *var,
                          const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  destroy_address(var, vdef);

  struct Address *a = address_create((char *) vdef->initial);

  *(struct Address **) var = a;
  return true;
}

void init_address(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_address = {
    "address",          set_address,   get_address,     set_native_address,
    get_native_address, reset_address, destroy_address,
  };
  cs_register_type(cs, DT_ADDRESS, &cst_address);
}

void address_free(struct Address **addr)
{
  if (!addr || !*addr)
    return; /* LCOV_EXCL_LINE */

  FREE(&(*addr)->personal);
  FREE(&(*addr)->mailbox);
  FREE(addr);
}
