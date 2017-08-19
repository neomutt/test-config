#include <stdio.h>
#include <stdbool.h>
#include "type/bool.h"
#include "config_set.h"
#include "debug.h"
#include "mutt_options.h"
#include "account.h"
#include "lib/lib.h"

const char *line = "--------------------------------------------------------------------------------";

bool VarApple;
bool VarBanana;
bool VarCherry;
bool VarDamson;
bool VarElderberry;
bool VarFig;
bool VarGuava;
bool VarHawthorn;
bool VarIlama;
bool VarJackfruit;

bool validator_fail(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *result)
{
  mutt_buffer_printf(result, "%s: %s, %ld", __func__, vdef->name, value);
  return false;
}

bool validator_succeed(struct ConfigSet *cs, const struct VariableDef *vdef, intptr_t value, struct Buffer *result)
{
  mutt_buffer_printf(result, "%s: %s, %ld", __func__, vdef->name, value);
  return true;
}

void log_line(const char *fn)
{
  int len = 52 - mutt_strlen(fn);
  printf("---- %s() %.*s\n", fn, len, line);
}

const struct VariableDef BoolVars[] = {
  { "Apple",      DT_BOOL, &VarApple,      0, NULL              }, /* bool_test_initial() */
  { "Banana",     DT_BOOL, &VarBanana,     1, NULL              },
  { "Cherry",     DT_BOOL, &VarCherry,     0, NULL              }, /* bool_test_basic_string_set */
  { "Damson",     DT_BOOL, &VarDamson,     0, NULL              }, /* bool_test_basic_string_get */
  { "Elderberry", DT_BOOL, &VarElderberry, 0, NULL              }, /* bool_test_basic_native_set */
  { "Fig",        DT_BOOL, &VarFig,        0, NULL              }, /* bool_test_basic_native_get */
  { "Guava",      DT_BOOL, &VarGuava,      0, NULL              }, /* bool_test_reset */
  { "Hawthorn",   DT_BOOL, &VarHawthorn,   0, validator_succeed }, /* bool_test_validator */
  { "Ilama",      DT_BOOL, &VarIlama,      0, validator_fail    },
  { "Jackfruit",  DT_BOOL, &VarJackfruit,  0                    }, /* bool_test_inherit */
  { NULL },
};

bool bool_listener(struct ConfigSet *cs, struct HashElem *he, const char *name, enum ConfigEvent ev)
{
  const char *events[] = { "set", "reset" };
  printf("Event: %s has been %s\n", name, events[ev]);
  return true;
}

void bool_list(struct ConfigSet *cs)
{
  log_line(__func__);
  cs_dump_set(cs);
}

bool bool_test_initial_values(struct ConfigSet *cs)
{
  log_line(__func__);
  printf("Apple = %d\n", VarApple);
  printf("Banana = %d\n", VarBanana);
  return ((VarApple == false) && (VarBanana == true));
}

bool bool_test_basic_string_set(struct ConfigSet *cs)
{
  log_line(__func__);
  bool result = false;

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  const char *valid[] = { "no", "yes", "n", "y", "false", "true", "0", "1", "off", "on", };
  const char *invalid[] = { "nope", "ye", "", NULL, };
  char *name = "Cherry";

  for (int i = 0; i < mutt_array_size(valid); i++)
  {
    VarCherry = ((i + 1) % 2);

    mutt_buffer_reset(&err);
    if (!cs_set_variable(cs, name, valid[i], &err))
    {
      printf("%s\n", err.data);
      goto btbss_out;
    }

    if (VarCherry != (i % 2))
    {
      printf("Value of %s wasn't changed\n", name);
      goto btbss_out;
    }
    printf("%s = %d, set by '%s'\n", name, VarCherry, valid[i]);
  }

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

  result = true;
btbss_out:
  FREE(&err.data);
  return result;
}

bool bool_test_basic_string_get(struct ConfigSet *cs)
{
  log_line(__func__);
  bool result = false;
  const char *name = "Damson";

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  VarDamson = false;
  mutt_buffer_reset(&err);
  if (!cs_get_variable(cs, name, &err))
  {
    printf("Get failed: %s\n", err.data);
    goto btbsg_out;
  }
  printf("%s = %d, %s\n", name, VarDamson, err.data);

  VarDamson = true;
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

bool bool_test_basic_native_set(struct ConfigSet *cs)
{
  log_line(__func__);
  bool result = false;
  char *name = "Elderberry";
  bool value = true;

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  VarElderberry = false;
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

  int invalid[] = { -1, 2 };
  for (int i = 0; i < mutt_array_size(invalid); i++)
  {
    VarElderberry = false;
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

bool bool_test_basic_native_get(struct ConfigSet *cs)
{
  log_line(__func__);
  bool result = false;
  char *name = "Fig";

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  VarFig = true;
  mutt_buffer_reset(&err);
  intptr_t value = cs_str_get_value(cs, name, &err);
  if (value != true)
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

bool bool_test_reset(struct ConfigSet *cs)
{
  log_line(__func__);
  bool result = false;

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  char *name = "Guava";
  VarGuava = true;
  mutt_buffer_reset(&err);

  if (!cs_reset_variable(cs, name, &err))
  {
    printf("%s\n", err.data);
    goto btr_out;
  }

  if (VarGuava == true)
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

bool bool_test_validator(struct ConfigSet *cs)
{
  log_line(__func__);
  bool result = false;

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  char *name = "Hawthorn";
  VarHawthorn = false;
  mutt_buffer_reset(&err);
  if (cs_set_variable(cs, name, "yes", &err))
  {
    printf("%s\n", err.data);
  }
  else
  {
    printf("%s\n", err.data);
    goto btv_out;
  }
  printf("String: %s = %d\n", name, VarHawthorn);

  VarHawthorn = false;
  mutt_buffer_reset(&err);
  if (cs_str_set_value(cs, name, 1, &err))
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
  VarIlama = false;
  mutt_buffer_reset(&err);
  if (!cs_set_variable(cs, name, "yes", &err))
  {
    printf("Expected error: %s\n", err.data);
  }
  else
  {
    printf("%s\n", err.data);
    goto btv_out;
  }
  printf("String: %s = %d\n", name, VarIlama);

  VarIlama = false;
  mutt_buffer_reset(&err);
  if (!cs_str_set_value(cs, name, 1, &err))
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

bool bool_test_inherit(struct ConfigSet *cs)
{
  log_line(__func__);
  bool result = false;

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;

  const char *account = "apple";
  const char *parent = "Jackfruit";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarStr[] = { parent, NULL, };

  struct Account *ac = ac_create(cs, account,  AccountVarStr);

  // set parent
  VarJackfruit = false;
  mutt_buffer_reset(&err);
  if (!cs_set_variable(cs, parent, "1", &err))
  {
    printf("Error: %s\n", err.data);
    goto bti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(&err);
  if (!cs_set_variable(cs, child, "0", &err))
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

bool bool_test(void)
{
  printf("%s\n", line);

  // struct Buffer err;
  // mutt_buffer_init(&err);
  // err.data = safe_calloc(1, STRING);
  // err.dsize = STRING;
  // mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new_set(30);

  init_bool(cs);
  if (!cs_register_variables(cs, BoolVars))
    return false;

  cs_add_listener(cs, bool_listener);

  bool_list(cs);

  if (!bool_test_initial_values(cs))   return false;
  if (!bool_test_basic_string_set(cs)) return false;
  if (!bool_test_basic_string_get(cs)) return false;
  if (!bool_test_basic_native_set(cs)) return false;
  if (!bool_test_basic_native_get(cs)) return false;
  if (!bool_test_reset(cs))            return false;
  if (!bool_test_validator(cs))        return false;
  if (!bool_test_inherit(cs))          return false;

  // hash_dump(cs->hash);

  // test_set_reset(cs);
  // test_validators(cs);
  // test_native(cs);

  cs_free(&cs);
  // FREE(&err.data);

  return true;
}

