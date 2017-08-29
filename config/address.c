#include "config.h"
#include <stddef.h>
#include <stdint.h>
#include "address.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "set.h"
#include "types.h"

static void address_destroy(void *var, const struct VariableDef *vdef)
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

static bool address_string_set(const struct ConfigSet *cs, void *var,
                               const struct VariableDef *vdef,
                               const char *value, struct Buffer *err)
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
    address_destroy(&addr, vdef);
    return false;
  }

  address_destroy(var, vdef);

  *(struct Address **) var = addr;
  return true;
}

static bool address_string_get(void *var, const struct VariableDef *vdef, struct Buffer *result)
{
  if (!var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  struct Address *a = *(struct Address **) var;
  if (!a)
    return true; /* empty string */

  mutt_buffer_addstr(result, a->personal);
  return true;
}

static struct Address *address_dup(struct Address *addr)
{
  if (!addr)
    return NULL; /* LCOV_EXCL_LINE */

  struct Address *a = safe_calloc(1, sizeof(*a));
  a->personal = safe_strdup(addr->personal);
  a->mailbox = safe_strdup(addr->mailbox);
  return a;
}

static bool address_native_set(const struct ConfigSet *cs, void *var,
                               const struct VariableDef *vdef, intptr_t value,
                               struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  if (vdef->validator && !vdef->validator(cs, vdef, value, err))
    return false;

  address_free(var);

  *(struct Address **) var = address_dup((struct Address *) value);
  return true;
}

static intptr_t address_native_get(const struct ConfigSet *cs, void *var,
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

static bool address_reset(const struct ConfigSet *cs, void *var,
                          const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return false; /* LCOV_EXCL_LINE */

  address_destroy(var, vdef);

  struct Address *a = address_create((char *) vdef->initial);

  *(struct Address **) var = a;
  return true;
}

void address_init(struct ConfigSet *cs)
{
  const struct ConfigSetType cst_address = {
    "address",          address_string_set, address_string_get,
    address_native_set, address_native_get, address_reset,
    address_destroy,
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
