#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "config/quad.h"
#include "account.h"
#include "config_set.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "mutt_options.h"
#include "test/common.h"

static short VarApple;
static short VarBanana;
static short VarCherry;
static short VarDamson;
static short VarElderberry;
static short VarFig;
static short VarGuava;
static short VarHawthorn;
static short VarIlama;
static short VarJackfruit;

struct VariableDef QuadVars[] = {
  { "Apple",      DT_QUAD, &VarApple,      0, NULL              }, /* test_initial() */
  { "Banana",     DT_QUAD, &VarBanana,     3, NULL              },
  { "Cherry",     DT_QUAD, &VarCherry,     0, NULL              }, /* test_basic_string_set */
  { "Damson",     DT_QUAD, &VarDamson,     0, NULL              }, /* test_basic_string_get */
  { "Elderberry", DT_QUAD, &VarElderberry, 0, NULL              }, /* test_basic_native_set */
  { "Fig",        DT_QUAD, &VarFig,        0, NULL              }, /* test_basic_native_get */
  { "Guava",      DT_QUAD, &VarGuava,      0, NULL              }, /* test_reset */
  { "Hawthorn",   DT_QUAD, &VarHawthorn,   0, validator_succeed }, /* test_validator */
  { "Ilama",      DT_QUAD, &VarIlama,      0, validator_fail    },
  { "Jackfruit",  DT_QUAD, &VarJackfruit,  0, NULL              }, /* test_inherit */
  { NULL },
};

static bool test_initial_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  printf("Apple = %d\n", VarApple);
  printf("Banana = %d\n", VarBanana);
  return ((VarApple == 0) && (VarBanana == 3));
}

static bool test_basic_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "no", "yes", "ask-no", "ask-yes" };
  const char *invalid[] = {
    "nope", "ye", "", NULL,
  };
  char *name = "Cherry";

  for (int i = 0; i < mutt_array_size(valid); i++)
  {
    VarCherry = ((i + 1) % 4);

    mutt_buffer_reset(err);
    if (!cs_set_variable(cs, name, valid[i], err))
    {
      printf("%s\n", err->data);
      return false;
    }

    if (VarCherry == ((i + 1) % 4))
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = %d, set by '%s'\n", name, VarCherry, valid[i]);
  }

  for (int i = 0; i < mutt_array_size(invalid); i++)
  {
    mutt_buffer_reset(err);
    if (!cs_set_variable(cs, name, invalid[i], err))
    {
      printf("Expected error: %s\n", err->data);
    }
    else
    {
      printf("%s = %d, set by '%s'\n", name, VarCherry, invalid[i]);
      printf("This test should have failed\n");
      return false;
    }
  }

  return true;
}

static bool test_basic_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Damson";

  int valid[] = { MUTT_NO, MUTT_YES, MUTT_ASKNO, MUTT_ASKYES };

  for (int i = 0; i < mutt_array_size(valid); i++)
  {
    VarDamson = i;
    mutt_buffer_reset(err);
    if (!cs_get_variable(cs, name, err))
    {
      printf("Get failed: %s\n", err->data);
      return false;
    }
    printf("%s = %d, %s\n", name, VarDamson, err->data);
  }

  return true;
}

static bool test_basic_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Elderberry";
  short value = MUTT_YES;

  VarElderberry = MUTT_NO;
  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, value, err))
  {
    printf("%s\n", err->data);
    return false;
  }

  if (VarElderberry != value)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("%s = %d, set to '%d'\n", name, VarElderberry, value);

  int invalid[] = { -1, 4 };
  for (int i = 0; i < mutt_array_size(invalid); i++)
  {
    VarElderberry = MUTT_NO;
    mutt_buffer_reset(err);
    if (!cs_str_set_value(cs, name, invalid[i], err))
    {
      printf("Expected error: %s\n", err->data);
    }
    else
    {
      printf("%s = %d, set by '%d'\n", name, VarElderberry, invalid[i]);
      printf("This test should have failed\n");
      return false;
    }
  }

  return true;
}

static bool test_basic_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Fig";

  VarFig = MUTT_YES;
  mutt_buffer_reset(err);
  intptr_t value = cs_str_get_value(cs, name, err);
  if (value != MUTT_YES)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = %ld\n", name, value);

  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Guava";
  VarGuava = MUTT_YES;
  mutt_buffer_reset(err);

  if (!cs_reset_variable(cs, name, err))
  {
    printf("%s\n", err->data);
    return false;
  }

  if (VarGuava == MUTT_YES)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("Reset: %s = %d\n", name, VarGuava);

  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Hawthorn";
  VarHawthorn = MUTT_NO;
  mutt_buffer_reset(err);
  if (cs_set_variable(cs, name, "yes", err))
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %d\n", name, VarHawthorn);

  VarHawthorn = MUTT_NO;
  mutt_buffer_reset(err);
  if (cs_str_set_value(cs, name, 1, err))
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("Native: %s = %d\n", name, VarHawthorn);

  name = "Ilama";
  VarIlama = MUTT_NO;
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, name, "yes", err))
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %d\n", name, VarIlama);

  VarIlama = MUTT_NO;
  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, 1, err))
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("Native: %s = %d\n", name, VarIlama);

  return true;
}

static void dump_native(struct ConfigSet *cs, const char *parent, const char *child)
{
  intptr_t pval = cs_str_get_value(cs, parent, NULL);
  intptr_t cval = cs_str_get_value(cs, child, NULL);

  printf("%15s = %ld\n", parent, pval);
  printf("%15s = %ld\n", child, cval);
}

static bool test_inherit(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  bool result = false;

  const char *account = "fruit";
  const char *parent = "Jackfruit";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarStr[] = {
    parent, NULL,
  };

  struct Account *ac = ac_create(cs, account, AccountVarStr);

  // set parent
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, parent, "yes", err))
  {
    printf("Error: %s\n", err->data);
    goto bti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, child, "no", err))
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

bool quad_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new_set(30);

  init_quad(cs);
  if (!cs_register_variables(cs, QuadVars))
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
