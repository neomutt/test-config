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
static char *VarCherry;
static char *VarElderberry;
static char *VarGuava;
static char *VarIlama;

static struct VariableDef Vars[] = {
  { "Apple",      DT_STR, &VarApple,      IP "apple",      NULL, },
  { "Banana",     DT_SYN, NULL,           IP "Apple",      NULL, },
  { "Cherry",     DT_STR, &VarCherry,     0,               NULL, },
  { "Damson",     DT_SYN, NULL,           IP "Cherry",     NULL, },
  { "Elderberry", DT_STR, &VarElderberry, 0,               NULL, },
  { "Fig",        DT_SYN, NULL,           IP "Elderberry", NULL, },
  { "Guava",      DT_STR, &VarGuava,      0,               NULL, },
  { "Hawthorn",   DT_SYN, NULL,           IP "Guava",      NULL, },
  { "Ilama",      DT_STR, &VarIlama,      IP "iguana",     NULL  },
  { "Jackfruit",  DT_SYN, NULL,           IP "Ilama",      NULL  },
  { NULL },
};

static struct VariableDef Vars2[] = {
  { "Jackfruit",  DT_SYN, NULL,           IP "Broken",     NULL  },
  { NULL },
};

static bool test_basic_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *name = "Damson";
  const char *value = "pudding";

  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, name, value, err))
  {
    printf("%s\n", err->data);
    return false;
  }

  if (mutt_strcmp(VarCherry, value) != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }
  printf("%s = %s, set by '%s'\n", name, NONULL(VarCherry), value);

  return true;
}

static bool test_basic_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Banana";

  mutt_buffer_reset(err);
  if (!cs_get_variable(cs, name, err))
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, NONULL(VarApple), err->data);

  return true;
}

static bool test_basic_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Fig";
  char *value = "tree";

  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, (intptr_t) value, err))
  {
    printf("%s\n", err->data);
    return false;
  }

  if (mutt_strcmp(VarElderberry, value) != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }
  printf("%s = %s, set by '%s'\n", name, NONULL(VarElderberry), value);

  return true;
}

static bool test_basic_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Hawthorn";

  if (!cs_set_variable(cs, name, "tree", err))
    return false;

  mutt_buffer_reset(err);
  intptr_t value = cs_str_get_value(cs, name, err);
  if (mutt_strcmp(VarGuava, (const char *) value) != 0)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = '%s', '%s'\n", name, VarGuava, (const char *) value);

  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Jackfruit";
  mutt_buffer_reset(err);

  printf("Initial: %s = '%s'\n", name, NONULL(VarIlama));
  if (!cs_set_variable(cs, name, "hello", err))
    return false;
  printf("Set: %s = '%s'\n", name, VarIlama);

  mutt_buffer_reset(err);
  if (!cs_reset_variable(cs, name, err))
  {
    printf("%s\n", err->data);
    return false;
  }

  if (mutt_strcmp(VarIlama, "iguana") != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("Reset: %s = '%s'\n", name, VarIlama);

  return true;
}

bool synonym_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new_set(30);

  init_string(cs);
  if (!cs_register_variables(cs, Vars))
    return false;

  if (!cs_register_variables(cs, Vars2))
  {
    printf("Expected error\n");
  }
  else
  {
    printf("Test should have failed\n");
    return false;
  }

  cs_add_listener(cs, log_listener);

  set_list(cs);

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

  cs_free(&cs);
  FREE(&err.data);

  return true;
}
