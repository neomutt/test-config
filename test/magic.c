#include <stdio.h>
#include <stdbool.h>
#include "type/magic.h"
#include "config_set.h"
#include "debug.h"
#include "mutt_options.h"
#include "account.h"
#include "lib/lib.h"
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

const struct VariableDef MagicVars[] = {
  { "Apple",      DT_MAGIC, &VarApple,      1, NULL              }, /* test_initial() */
  { "Banana",     DT_MAGIC, &VarBanana,     3, NULL              },
  { "Cherry",     DT_MAGIC, &VarCherry,     1, NULL              }, /* test_basic_string_set */
  { "Damson",     DT_MAGIC, &VarDamson,     1, NULL              }, /* test_basic_string_get */
  { "Elderberry", DT_MAGIC, &VarElderberry, 1, NULL              }, /* test_basic_native_set */
  { "Fig",        DT_MAGIC, &VarFig,        1, NULL              }, /* test_basic_native_get */
  { "Guava",      DT_MAGIC, &VarGuava,      1, NULL              }, /* test_reset */
  { "Hawthorn",   DT_MAGIC, &VarHawthorn,   1, validator_succeed }, /* test_validator */
  { "Ilama",      DT_MAGIC, &VarIlama,      1, validator_fail    },
  { "Jackfruit",  DT_MAGIC, &VarJackfruit,  1, NULL              }, /* test_inherit */
  { NULL },
};

static bool test_initial_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  printf("Apple = %d\n", VarApple);
  printf("Banana = %d\n", VarBanana);
  return ((VarApple == 1) && (VarBanana == 3));
}

static bool test_basic_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "mbox", "mmdf", "mh", "maildir" };
  const char *invalid[] = { "mbox2", "mm", "", NULL, };
  char *name = "Cherry";

  for (int i = 0; i < mutt_array_size(valid); i++)
  {
    VarCherry = ((i + 1) % 4) + 1;

    mutt_buffer_reset(err);
    if (!cs_set_variable(cs, name, valid[i], err))
    {
      printf("%s\n", err->data);
      return false;
    }

    if (VarCherry == (((i + 1) % 4) + 1))
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

  int valid[] = { MUTT_MBOX, MUTT_MMDF, MUTT_MH, MUTT_MAILDIR };

  for (int i = 0; i < mutt_array_size(valid); i++)
  {
    VarDamson = valid[i];
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
  short value = MUTT_MAILDIR;

  VarElderberry = MUTT_MBOX;
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

  int invalid[] = { 0, 5 };
  for (int i = 0; i < mutt_array_size(invalid); i++)
  {
    VarElderberry = MUTT_MBOX;
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

  VarFig = MUTT_MAILDIR;
  mutt_buffer_reset(err);
  intptr_t value = cs_str_get_value(cs, name, err);
  if (value != MUTT_MAILDIR)
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
  VarGuava = MUTT_MAILDIR;
  mutt_buffer_reset(err);

  if (!cs_reset_variable(cs, name, err))
  {
    printf("%s\n", err->data);
    return false;
  }

  if (VarGuava == MUTT_MAILDIR)
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
  VarHawthorn = MUTT_MBOX;
  mutt_buffer_reset(err);
  if (cs_set_variable(cs, name, "maildir", err))
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %d\n", name, VarHawthorn);

  VarHawthorn = MUTT_MBOX;
  mutt_buffer_reset(err);
  if (cs_str_set_value(cs, name, MUTT_MAILDIR, err))
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
  VarIlama = MUTT_MBOX;
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, name, "maildir", err))
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %d\n", name, VarIlama);

  VarIlama = MUTT_MBOX;
  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, MUTT_MAILDIR, err))
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
  intptr_t cval = cs_str_get_value(cs, child,  NULL);

  printf("%15s = %ld\n", parent, pval);
  printf("%15s = %ld\n", child,  cval);
}

static bool test_inherit(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  bool result = false;

  const char *account = "fruit";
  const char *parent = "Jackfruit";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarStr[] = { parent, NULL, };

  struct Account *ac = ac_create(cs, account,  AccountVarStr);

  // set parent
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, parent, "maildir", err))
  {
    printf("Error: %s\n", err->data);
    goto bti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, child, "mh", err))
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

bool magic_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new_set(30);

  init_magic(cs);
  if (!cs_register_variables(cs, MagicVars))
    return false;

  cs_add_listener(cs, log_listener);

  set_list(cs);

  if (!test_initial_values(cs, &err))   return false;
  if (!test_basic_string_set(cs, &err)) return false;
  if (!test_basic_string_get(cs, &err)) return false;
  if (!test_basic_native_set(cs, &err)) return false;
  if (!test_basic_native_get(cs, &err)) return false;
  if (!test_reset(cs, &err))            return false;
  if (!test_validator(cs, &err))        return false;
  if (!test_inherit(cs, &err))          return false;

  cs_free(&cs);
  FREE(&err.data);

  return true;
}
