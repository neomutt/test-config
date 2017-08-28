#include "config.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "config/address.h"
#include "config/account.h"
#include "config/config_set.h"
#include "config/types.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "mutt_options.h"
#include "test/common.h"


static struct Address *VarApple;
static struct Address *VarBanana;
static struct Address *VarCherry;
static struct Address *VarDamson;
static struct Address *VarElderberry;
static struct Address *VarFig;
static struct Address *VarGuava;
static struct Address *VarHawthorn;
static struct Address *VarIlama;
static struct Address *VarJackfruit;
static struct Address *VarKumquat;
static struct Address *VarLemon;
static struct Address *VarMango;
static struct Address *VarNectarine;

// clang-format off
static struct VariableDef Vars[] = {
  { "Apple",      DT_ADDR, &VarApple,      IP "apple@example.com",   NULL              }, /* test_initial_values() */
  { "Banana",     DT_ADDR, &VarBanana,     IP "banana@example.com",  NULL              },
  { "Cherry",     DT_ADDR, &VarCherry,     0,                        NULL              }, /* test_basic_address_set */
  { "Damson",     DT_ADDR, &VarDamson,     IP "damson@example.com",  NULL              },
  { "Elderberry", DT_ADDR, &VarElderberry, 0,                        NULL              }, /* test_basic_address_get */
  { "Fig",        DT_ADDR, &VarFig,        IP "fig@example.com",     NULL              },
  { "Guava",      DT_ADDR, &VarGuava,      0,                        NULL              },
  { "Hawthorn",   DT_ADDR, &VarHawthorn,   0,                        NULL              }, /* test_basic_native_set */
  { "Ilama",      DT_ADDR, &VarIlama,      IP "ilama@example.com",   NULL              },
  { "Jackfruit",  DT_ADDR, &VarJackfruit,  0,                        NULL              }, /* test_basic_native_get */
  { "Kumquat",    DT_ADDR, &VarKumquat,    IP "kumquat@example.com", NULL              }, /* test_reset */
  { "Lemon",      DT_ADDR, &VarLemon,      IP "lemon@example.com",   validator_succeed }, /* test_validator */
  { "Mango",      DT_ADDR, &VarMango,      IP "mango@example.com",   validator_fail    },
  { "Nectarine",  DT_ADDR, &VarNectarine,  0,                        NULL              }, /* test_inherit */
  { NULL },
};
// clang-format on

static bool test_initial_values_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  printf("Apple = %s\n", VarApple->personal);
  printf("Banana = %s\n", VarBanana->personal);

  return ((mutt_strcmp(VarApple->personal, "apple@example.com") == 0) &&
          (mutt_strcmp(VarBanana->personal, "banana@example.com") == 0));
}

static bool test_basic_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "hello@example.com", "world@example.com", NULL };
  char *name = "Cherry";
  char *addr = NULL;

  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    if (!cs_set_variable(cs, name, valid[i], err))
    {
      printf("%s\n", err->data);
      return false;
    }

    addr = VarCherry ? VarCherry->personal : NULL;
    if (mutt_strcmp(addr, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(addr), NONULL(valid[i]));
  }

  name = "Damson";
  for (unsigned int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    if (!cs_set_variable(cs, name, valid[i], err))
    {
      printf("%s\n", err->data);
      return false;
    }

    addr = VarDamson ? VarDamson->personal : NULL;
    if (mutt_strcmp(addr, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(addr), NONULL(valid[i]));
  }

  return true;
}

static bool test_basic_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Elderberry";
  char *addr = NULL;

  mutt_buffer_reset(err);
  if (!cs_get_variable(cs, name, err))
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  addr = VarElderberry ? VarElderberry->personal : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(addr), err->data);

  name = "Fig";
  mutt_buffer_reset(err);
  if (!cs_get_variable(cs, name, err))
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  addr = VarFig ? VarFig->personal : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(addr), err->data);

  name = "Guava";
  if (!cs_set_variable(cs, name, "guava", err))
    return false;

  mutt_buffer_reset(err);
  if (!cs_get_variable(cs, name, err))
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  addr = VarGuava ? VarGuava->personal : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(addr), err->data);

  return true;
}

static bool test_basic_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  struct Address *a = addr_create("hello@example.com");
  char *name = "Hawthorn";
  char *addr = NULL;
  bool result = false;

  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, (intptr_t) a, err))
  {
    printf("%s\n", err->data);
    goto tbns_out;
  }

  addr = VarHawthorn ? VarHawthorn->personal : NULL;
  if (mutt_strcmp(addr, a->personal) != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    goto tbns_out;
  }
  printf("%s = '%s', set by '%s'\n", name, NONULL(addr), a->personal);

  name = "Ilama";
  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, 0, err))
  {
    printf("%s\n", err->data);
    goto tbns_out;
  }

  if (VarIlama != NULL)
  {
    printf("Value of %s wasn't changed\n", name);
    goto tbns_out;
  }
  addr = VarIlama ? VarIlama->personal : NULL;
  printf("%s = '%s', set by NULL\n", name, NONULL(addr));

  result = true;
tbns_out:
  addr_free(&a);
  return result;
}

static bool test_basic_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Jackfruit";

  if (!cs_set_variable(cs, name, "jackfruit@example.com", err))
    return false;

  mutt_buffer_reset(err);
  intptr_t value = cs_str_get_value(cs, name, err);
  struct Address *a = (struct Address *) value;

  if (VarJackfruit != a)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  char *addr1 = VarJackfruit ? VarJackfruit->personal : NULL;
  char *addr2 = a ? a->personal : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(addr1), NONULL(addr2));

  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Kumquat";
  char *addr = NULL;

  mutt_buffer_reset(err);

  addr = VarKumquat ? VarKumquat->personal : NULL;
  printf("Initial: %s = '%s'\n", name, NONULL(addr));
  if (!cs_set_variable(cs, name, "hello@example.com", err))
    return false;
  addr = VarKumquat ? VarKumquat->personal : NULL;
  printf("Set: %s = '%s'\n", name, NONULL(addr));

  if (!cs_reset_variable(cs, name, err))
  {
    printf("%s\n", err->data);
    return false;
  }

  addr = VarKumquat ? VarKumquat->personal : NULL;
  if (mutt_strcmp(addr, "kumquat@example.com") != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("Reset: %s = '%s'\n", name, NONULL(addr));

  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Lemon";
  char *addr = NULL;
  struct Address *a = addr_create("world@example.com");
  bool result = false;

  mutt_buffer_reset(err);
  if (cs_set_variable(cs, name, "hello@example.com", err))
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  addr = VarLemon ? VarLemon->personal : NULL;
  printf("Address: %s = %s\n", name, NONULL(addr));

  mutt_buffer_reset(err);
  if (cs_str_set_value(cs, name, IP a, err))
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  addr = VarLemon ? VarLemon->personal : NULL;
  printf("Native: %s = %s\n", name, NONULL(addr));

  name = "Mango";
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, name, "hello@example.com", err))
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  addr = VarMango ? VarMango->personal : NULL;
  printf("Address: %s = %s\n", name, NONULL(addr));

  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, IP a, err))
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    goto tv_out;
  }
  addr = VarMango ? VarMango->personal : NULL;
  printf("Native: %s = %s\n", name, NONULL(addr));

  result = true;
tv_out:
  addr_free(&a);
  return result;
}

static void dump_native(struct ConfigSet *cs, const char *parent, const char *child)
{
  intptr_t pval = cs_str_get_value(cs, parent, NULL);
  intptr_t cval = cs_str_get_value(cs, child, NULL);

  struct Address *pa = (struct Address *) pval;
  struct Address *ca = (struct Address *) cval;

  char *pstr = pa ? pa->personal : NULL;
  char *cstr = ca ? ca->personal : NULL;

  printf("%15s = %s\n", parent, NONULL(pstr));
  printf("%15s = %s\n", child, NONULL(cstr));
}

static bool test_inherit(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  bool result = false;

  const char *account = "fruit";
  const char *parent = "Nectarine";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarAddr[] = {
    parent, NULL,
  };

  struct Account *ac = ac_create(cs, account, AccountVarAddr);

  // set parent
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, parent, "hello@example.com", err))
  {
    printf("Error: %s\n", err->data);
    goto bti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, child, "world@example.com", err))
  {
    printf("Error: %s\n", err->data);
    goto bti_out;
  }
  dump_native(cs, parent, child);

  // reset child
  mutt_buffer_reset(err);
  if (!cs_reset_variable(cs, child, err))
  {
    printf("Error: %s\n", err->data);
    goto bti_out;
  }
  dump_native(cs, parent, child);

  // reset parent
  mutt_buffer_reset(err);
  if (!cs_reset_variable(cs, parent, err))
  {
    printf("Error: %s\n", err->data);
    goto bti_out;
  }
  dump_native(cs, parent, child);

  result = true;
bti_out:
  ac_free(cs, &ac);
  return result;
}

bool address_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new_set(30);

  init_addr(cs);
  if (!cs_register_variables(cs, Vars))
    return false;

  cs_add_listener(cs, log_listener);

  set_list(cs);

  if (!test_initial_values_values(cs, &err))
    return false;
  if (!test_basic_string_set(cs, &err))
    return false;
  if (!test_basic_string_get(cs, &err))
    return false;
  if (!test_basic_native_set(cs, &err))
    return false;
  if (!test_basic_native_get(cs, &err))
    return false;
  if (!test_reset(cs, &err))
    return false;
  if (!test_validator(cs, &err))
    return false;
  if (!test_inherit(cs, &err))
    return false;

  cs_free(&cs);
  FREE(&err.data);

  return true;
}
