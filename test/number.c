#include <stdio.h>
#include <stdbool.h>
#include "type/number.h"
#include "config_set.h"
#include "debug.h"
#include "mutt_options.h"
#include "account.h"
#include "lib/lib.h"
#include "test/common.h"

short VarApple;
short VarBanana;
short VarCherry;
short VarDamson;
short VarElderberry;
short VarFig;
short VarGuava;
short VarHawthorn;
short VarIlama;
short VarJackfruit;

const struct VariableDef NumberVars[] = {
  { "Apple",      DT_NUM, &VarApple,      -42, NULL              }, /* number_test_initial() */
  { "Banana",     DT_NUM, &VarBanana,      99, NULL              },
  { "Cherry",     DT_NUM, &VarCherry,       0, NULL              }, /* number_test_basic_string_set */
  { "Damson",     DT_NUM, &VarDamson,       0, NULL              }, /* number_test_basic_string_get */
  { "Elderberry", DT_NUM, &VarElderberry,   0, NULL              }, /* number_test_basic_native_set */
  { "Fig",        DT_NUM, &VarFig,          0, NULL              }, /* number_test_basic_native_get */
  { "Guava",      DT_NUM, &VarGuava,       99, NULL              }, /* number_test_reset */
  { "Hawthorn",   DT_NUM, &VarHawthorn,     0, validator_succeed }, /* number_test_validator */
  { "Ilama",      DT_NUM, &VarIlama,        0, validator_fail    },
  { "Jackfruit",  DT_NUM, &VarJackfruit,    0, NULL              }, /* number_test_inherit */
  { NULL },
};

bool number_test_initial_values(struct ConfigSet *cs)
{
  log_line(__func__);
  printf("Apple = %d\n", VarApple);
  printf("Banana = %d\n", VarBanana);
  return ((VarApple == -42) && (VarBanana == 99));
}

bool number_test_basic_string_set(struct ConfigSet *cs)
{
  log_line(__func__);
  bool result = false;

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  const char *valid[] = { "-123", "0", "456" };
  int numbers[] = { -123, 0, 456 };
  const char *invalid[] = { "-32769", "32768", "junk", NULL, };
  char *name = "Cherry";

  for (int i = 0; i < mutt_array_size(valid); i++)
  {
    VarCherry = -42;

    mutt_buffer_reset(&err);
    if (!cs_set_variable(cs, name, valid[i], &err))
    {
      printf("%s\n", err.data);
      goto btbss_out;
    }

    if (VarCherry != numbers[i])
    {
      printf("Value of %s wasn't changed\n", name);
      goto btbss_out;
    }
    printf("%s = %d, set by '%s'\n", name, VarCherry, valid[i]);
  }

  printf("\n");
  for (int i = 0; i < mutt_array_size(invalid); i++)
  {
    mutt_buffer_reset(&err);
    if (!cs_set_variable(cs, name, invalid[i], &err))
    {
      printf("Expected error: %s\n", err.data);
    }
    else
    {
      printf("%s = %d, set by '%s'\n", name, VarCherry, invalid[i]);
      printf("This test should have failed\n");
      goto btbss_out;
    }
  }
  printf("\n");

  result = true;
btbss_out:
  FREE(&err.data);
  return result;
}

bool number_test_basic_string_get(struct ConfigSet *cs)
{
  log_line(__func__);
  bool result = false;
  const char *name = "Damson";

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  VarDamson = 123;
  mutt_buffer_reset(&err);
  if (!cs_get_variable(cs, name, &err))
  {
    printf("Get failed: %s\n", err.data);
    goto btbsg_out;
  }
  printf("%s = %d, %s\n", name, VarDamson, err.data);

  VarDamson = -789;
  mutt_buffer_reset(&err);
  if (!cs_get_variable(cs, name, &err))
  {
    printf("Get failed: %s\n", err.data);
    goto btbsg_out;
  }
  printf("%s = %d, %s\n", name, VarDamson, err.data);

  result = true;
btbsg_out:
  FREE(&err.data);
  return result;
}

bool number_test_basic_native_set(struct ConfigSet *cs)
{
  log_line(__func__);
  bool result = false;
  char *name = "Elderberry";
  short value = 12345;

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  VarElderberry = 0;
  mutt_buffer_reset(&err);
  if (!cs_str_set_value(cs, name, value, &err))
  {
    printf("%s\n", err.data);
    goto btbns_out;
  }

  if (VarElderberry != value)
  {
    printf("Value of %s wasn't changed\n", name);
    goto btbns_out;
  }

  printf("%s = %d, set to '%d'\n", name, VarElderberry, value);

  int invalid[] = { -32769, 32768 };
  for (int i = 0; i < mutt_array_size(invalid); i++)
  {
    VarElderberry = 123;
    mutt_buffer_reset(&err);
    if (!cs_str_set_value(cs, name, invalid[i], &err))
    {
      printf("Expected error: %s\n", err.data);
    }
    else
    {
      printf("%s = %d, set by '%d'\n", name, VarElderberry, invalid[i]);
      printf("This test should have failed\n");
      goto btbns_out;
    }
  }

  result = true;
btbns_out:
  FREE(&err.data);
  return result;
}

bool number_test_basic_native_get(struct ConfigSet *cs)
{
  log_line(__func__);
  bool result = false;
  char *name = "Fig";

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  VarFig = 3456;
  mutt_buffer_reset(&err);
  intptr_t value = cs_str_get_value(cs, name, &err);
  if (value != 3456)
  {
    printf("Get failed: %s\n", err.data);
    goto btbng_out;
  }
  printf("%s = %ld\n", name, value);

  result = true;
btbng_out:
  FREE(&err.data);
  return result;
}

bool number_test_reset(struct ConfigSet *cs)
{
  log_line(__func__);
  bool result = false;

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  char *name = "Guava";
  VarGuava = 345;
  mutt_buffer_reset(&err);

  if (!cs_reset_variable(cs, name, &err))
  {
    printf("%s\n", err.data);
    goto btr_out;
  }

  if (VarGuava == 345)
  {
    printf("Value of %s wasn't changed\n", name);
    goto btr_out;
  }

  printf("Reset: %s = %d\n", name, VarGuava);

  result = true;
btr_out:
  FREE(&err.data);
  return result;
}

bool number_test_validator(struct ConfigSet *cs)
{
  log_line(__func__);
  bool result = false;

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  char *name = "Hawthorn";
  VarHawthorn = 123;
  mutt_buffer_reset(&err);
  if (cs_set_variable(cs, name, "456", &err))
  {
    printf("%s\n", err.data);
  }
  else
  {
    printf("%s\n", err.data);
    goto btv_out;
  }
  printf("String: %s = %d\n", name, VarHawthorn);

  VarHawthorn = 456;
  mutt_buffer_reset(&err);
  if (cs_str_set_value(cs, name, 123, &err))
  {
    printf("%s\n", err.data);
  }
  else
  {
    printf("%s\n", err.data);
    goto btv_out;
  }
  printf("Native: %s = %d\n", name, VarHawthorn);

  name = "Ilama";
  VarIlama = 123;
  mutt_buffer_reset(&err);
  if (!cs_set_variable(cs, name, "456", &err))
  {
    printf("Expected error: %s\n", err.data);
  }
  else
  {
    printf("%s\n", err.data);
    goto btv_out;
  }
  printf("String: %s = %d\n", name, VarIlama);

  VarIlama = 456;
  mutt_buffer_reset(&err);
  if (!cs_str_set_value(cs, name, 123, &err))
  {
    printf("Expected error: %s\n", err.data);
  }
  else
  {
    printf("%s\n", err.data);
    goto btv_out;
  }
  printf("Native: %s = %d\n", name, VarIlama);

  result = true;
btv_out:
  FREE(&err.data);
  return result;
}

static void dump_native(struct ConfigSet *cs, const char *parent, const char *child)
{
  intptr_t pval = cs_str_get_value(cs, parent, NULL);
  intptr_t cval = cs_str_get_value(cs, child,  NULL);

  printf("%15s = %ld\n", parent, pval);
  printf("%15s = %ld\n", child,  cval);
}

bool number_test_inherit(struct ConfigSet *cs)
{
  log_line(__func__);
  bool result = false;

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  const char *account = "fruit";
  const char *parent = "Jackfruit";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarStr[] = { parent, NULL, };

  struct Account *ac = ac_create(cs, account,  AccountVarStr);

  // set parent
  VarJackfruit = 123;
  mutt_buffer_reset(&err);
  if (!cs_set_variable(cs, parent, "456", &err))
  {
    printf("Error: %s\n", err.data);
    goto bti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(&err);
  if (!cs_set_variable(cs, child, "-99", &err))
  {
    printf("Error: %s\n", err.data);
    goto bti_out;
  }
  dump_native(cs, parent, child);

  // reset child
  mutt_buffer_reset(&err);
  if (!cs_reset_variable(cs, child, &err))
  {
    printf("Error: %s\n", err.data);
    goto bti_out;
  }
  dump_native(cs, parent, child);

  // reset parent
  mutt_buffer_reset(&err);
  if (!cs_reset_variable(cs, parent, &err))
  {
    printf("Error: %s\n", err.data);
    goto bti_out;
  }
  dump_native(cs, parent, child);

  result = true;
bti_out:
  ac_free(cs, &ac);
  FREE(&err.data);
  return result;
}

bool number_test(void)
{
  printf("%s\n", line);

  // struct Buffer err;
  // mutt_buffer_init(&err);
  // err.data = safe_calloc(1, STRING);
  // err.dsize = STRING;
  // mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new_set(30);

  init_num(cs);
  if (!cs_register_variables(cs, NumberVars))
    return false;

  cs_add_listener(cs, log_listener);

  set_list(cs);

  if (!number_test_initial_values(cs))   return false;
  if (!number_test_basic_string_set(cs)) return false;
  if (!number_test_basic_string_get(cs)) return false;
  if (!number_test_basic_native_set(cs)) return false;
  if (!number_test_basic_native_get(cs)) return false;
  if (!number_test_reset(cs))            return false;
  if (!number_test_validator(cs))        return false;
  if (!number_test_inherit(cs))          return false;

  // hash_dump(cs->hash);

  // test_set_reset(cs);
  // test_validators(cs);
  // test_native(cs);

  cs_free(&cs);
  // FREE(&err.data);

  return true;
}

