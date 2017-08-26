#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "account.h"
#include "config_set.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "mutt_options.h"
#include "test/common.h"
#include "config/string3.h"

static char *VarApple;
static char *VarBanana;
static char *VarCherry;
static char *VarDamson;
static char *VarElderberry;
static char *VarFig;
static char *VarGuava;
static char *VarHawthorn;
static char *VarIlama;
static char *VarJackfruit;
static char *VarKumquat;
static char *VarLemon;
static char *VarMango;
static char *VarNectarine;

struct VariableDef StringVars[] = {
  { "Apple",      DT_STR, &VarApple,      IP "apple",   NULL              }, /* test_initial() */
  { "Banana",     DT_STR, &VarBanana,     IP "banana",  NULL              },
  { "Cherry",     DT_STR, &VarCherry,     0,            NULL              }, /* test_basic_string_set */
  { "Damson",     DT_STR, &VarDamson,     IP "damson",  NULL              },
  { "Elderberry", DT_STR, &VarElderberry, 0,            NULL              }, /* test_basic_string_get */
  { "Fig",        DT_STR, &VarFig,        IP "fig",     NULL              },
  { "Guava",      DT_STR, &VarGuava,      0,            NULL              },
  { "Hawthorn",   DT_STR, &VarHawthorn,   0,            NULL              }, /* test_basic_native_set */
  { "Ilama",      DT_STR, &VarIlama,      IP "ilama",   NULL              },
  { "Jackfruit",  DT_STR, &VarJackfruit,  0,            NULL              }, /* test_basic_native_get */
  { "Kumquat",    DT_STR, &VarKumquat,    IP "kumquat", NULL              }, /* test_reset */
  { "Lemon",      DT_STR, &VarLemon,      IP "lemon",   validator_succeed }, /* test_validator */
  { "Mango",      DT_STR, &VarMango,      IP "mango",   validator_fail    },
  { "Nectarine",  DT_STR, &VarNectarine,  0,            NULL              }, /* test_inherit */
  { NULL },
};

static bool test_initial_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  printf("Apple = %s\n", VarApple);
  printf("Banana = %s\n", VarBanana);

  return ((mutt_strcmp(VarApple, "apple") == 0) &&
          (mutt_strcmp(VarBanana, "banana") == 0));
}

static bool test_basic_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "hello", "world", NULL };
  char *name = "Cherry";

  for (int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    if (!cs_set_variable(cs, name, valid[i], err))
    {
      printf("%s\n", err->data);
      return false;
    }

    if (mutt_strcmp(VarCherry, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = %s, set by '%s'\n", name, NONULL(VarCherry), NONULL(valid[i]));
  }

  name = "Damson";
  for (int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    if (!cs_set_variable(cs, name, valid[i], err))
    {
      printf("%s\n", err->data);
      return false;
    }

    if (mutt_strcmp(VarDamson, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = %s, set by '%s'\n", name, NONULL(VarDamson), NONULL(valid[i]));
  }

  return true;
}

static bool test_basic_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Elderberry";

  mutt_buffer_reset(err);
  if (!cs_get_variable(cs, name, err))
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, NONULL(VarElderberry), err->data);

  name = "Fig";
  mutt_buffer_reset(err);
  if (!cs_get_variable(cs, name, err))
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, NONULL(VarFig), err->data);

  name = "Guava";
  if (!cs_set_variable(cs, name, "guava", err))
    return false;

  mutt_buffer_reset(err);
  if (!cs_get_variable(cs, name, err))
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, NONULL(VarGuava), err->data);

  return true;
}

static bool test_basic_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "hello", "world", NULL };
  char *name = "Hawthorn";

  for (int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    if (!cs_str_set_value(cs, name, (intptr_t) valid[i], err))
    {
      printf("%s\n", err->data);
      return false;
    }

    if (mutt_strcmp(VarHawthorn, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = %s, set by '%s'\n", name, NONULL(VarHawthorn), NONULL(valid[i]));
  }

  name = "Ilama";
  for (int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    if (!cs_str_set_value(cs, name, (intptr_t) valid[i], err))
    {
      printf("%s\n", err->data);
      return false;
    }

    if (mutt_strcmp(VarIlama, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = %s, set by '%s'\n", name, NONULL(VarIlama), NONULL(valid[i]));
  }

  return true;
}

static bool test_basic_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Jackfruit";

  if (!cs_set_variable(cs, name, "jackfruit", err))
    return false;

  mutt_buffer_reset(err);
  intptr_t value = cs_str_get_value(cs, name, err);
  if (mutt_strcmp(VarJackfruit, (char *) value) != 0)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, VarJackfruit, (char *) value);

  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Kumquat";
  mutt_buffer_reset(err);

  printf("Initial: %s = '%s'\n", name, VarKumquat);
  if (!cs_set_variable(cs, name, "hello", err))
    return false;
  printf("Set: %s = '%s'\n", name, VarKumquat);

  if (!cs_reset_variable(cs, name, err))
  {
    printf("%s\n", err->data);
    return false;
  }

  if (mutt_strcmp(VarKumquat, "kumquat") != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("Reset: %s = '%s'\n", name, VarKumquat);

  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Lemon";
  mutt_buffer_reset(err);
  if (cs_set_variable(cs, name, "hello", err))
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %s\n", name, VarLemon);

  mutt_buffer_reset(err);
  if (cs_str_set_value(cs, name, IP "world", err))
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("Native: %s = %s\n", name, VarLemon);

  name = "Mango";
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, name, "hello", err))
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %s\n", name, VarMango);

  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, IP "world", err))
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("Native: %s = %s\n", name, VarMango);

  return true;
}

static void dump_native(struct ConfigSet *cs, const char *parent, const char *child)
{
  intptr_t pval = cs_str_get_value(cs, parent, NULL);
  intptr_t cval = cs_str_get_value(cs, child, NULL);

  printf("%15s = %s\n", parent, (char *) pval);
  printf("%15s = %s\n", child, (char *) cval);
}

static bool test_inherit(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  bool result = false;

  const char *account = "fruit";
  const char *parent = "Nectarine";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarStr[] = {
    parent, NULL,
  };

  struct Account *ac = ac_create(cs, account, AccountVarStr);

  // set parent
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, parent, "hello", err))
  {
    printf("Error: %s\n", err->data);
    goto bti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, child, "world", err))
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

bool string_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new_set(30);

  init_string(cs);
  if (!cs_register_variables(cs, StringVars))
    return false;

  cs_add_listener(cs, log_listener);

  set_list(cs);

  if (!test_initial_values(cs, &err))
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
