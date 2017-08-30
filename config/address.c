#include "config.h"
#include <stddef.h>
#include <limits.h>
#include <stdint.h>
#include "address.h"
#include "lib/buffer.h"
#include "lib/debug.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "set.h"
#include "types.h"

static void address_destroy(const struct ConfigSet *cs, void *var,
                            const struct VariableDef *vdef)
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

static int address_string_set(const struct ConfigSet *cs, void *var,
                              const struct VariableDef *vdef, const char *value,
                              struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct Address *addr = NULL;

  // An empty address "" will be stored as NULL
  if (value && (value[0] != '\0'))
  {
    addr = safe_calloc(1, sizeof(*addr));
    addr->personal = safe_strdup((const char *) value);
    addr->mailbox = safe_strdup("dummy1");
  }

  int result = CSR_SUCCESS;
  if (vdef->validator)
    result = vdef->validator(cs, vdef, (intptr_t) addr, err);

  if ((result & CSR_RESULT_MASK) != CSR_SUCCESS)
  {
    address_destroy(cs, &addr, vdef);
    return result | CSR_INV_VALIDATOR;
  }

  address_destroy(cs, var, vdef);

  *(struct Address **) var = addr;
  return CSR_SUCCESS;
}

static int address_string_get(const struct ConfigSet *cs, void *var,
                              const struct VariableDef *vdef, struct Buffer *result)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct Address *a = *(struct Address **) var;
  if (!a)
    return CSR_SUCCESS | CSR_SUC_EMPTY; /* empty string */

  mutt_buffer_addstr(result, a->personal);
  return CSR_SUCCESS;
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

static int address_native_set(const struct ConfigSet *cs, void *var,
                              const struct VariableDef *vdef, intptr_t value,
                              struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  int result = CSR_SUCCESS;
  if (vdef->validator)
    result = vdef->validator(cs, vdef, value, err);

  if ((result & CSR_RESULT_MASK) != CSR_SUCCESS)
    return result | CSR_INV_VALIDATOR;

  address_free(var);

  *(struct Address **) var = address_dup((struct Address *) value);
  return CSR_SUCCESS;
}

static intptr_t address_native_get(const struct ConfigSet *cs, void *var,
                                   const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  struct Address *addr = *(struct Address **) var;

  return (intptr_t) addr;
}

static int address_reset(const struct ConfigSet *cs, void *var,
                         const struct VariableDef *vdef, struct Buffer *err)
{
  if (!cs || !var || !vdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  address_destroy(cs, var, vdef);

  struct Address *a = address_create((char *) vdef->initial);

  *(struct Address **) var = a;
  return CSR_SUCCESS;
}

struct Address *address_create(const char *addr)
{
  struct Address *a = safe_calloc(1, sizeof(*a));
  a->personal = safe_strdup(addr);
  a->mailbox = safe_strdup("dummy3");
  return a;
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
