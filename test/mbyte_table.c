#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "type/mbyte_table.h"
#include "account.h"
#include "config_set.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "mutt_options.h"
#include "test/common.h"

static struct MbCharTable *VarApple;
static struct MbCharTable *VarBanana;
static struct MbCharTable *VarCherry;
static struct MbCharTable *VarDamson;
static struct MbCharTable *VarElderberry;
static struct MbCharTable *VarFig;
static struct MbCharTable *VarGuava;
static struct MbCharTable *VarHawthorn;
static struct MbCharTable *VarIlama;
static struct MbCharTable *VarJackfruit;
static struct MbCharTable *VarKumquat;
static struct MbCharTable *VarLemon;
static struct MbCharTable *VarMango;
static struct MbCharTable *VarNectarine;

struct VariableDef MbCharTableVars[] = {
  { "Apple",      DT_MBCHARTBL, &VarApple,      IP "apple",   NULL              }, /* test_initial() */
  { "Banana",     DT_MBCHARTBL, &VarBanana,     IP "banana",  NULL              },
  { "Cherry",     DT_MBCHARTBL, &VarCherry,     0,            NULL              }, /* test_basic_mbytetable_set */
  { "Damson",     DT_MBCHARTBL, &VarDamson,     IP "damson",  NULL              },
  { "Elderberry", DT_MBCHARTBL, &VarElderberry, 0,            NULL              }, /* test_basic_mbytetable_get */
  { "Fig",        DT_MBCHARTBL, &VarFig,        IP "fig",     NULL              },
  { "Guava",      DT_MBCHARTBL, &VarGuava,      0,            NULL              },
  { "Hawthorn",   DT_MBCHARTBL, &VarHawthorn,   0,            NULL              }, /* test_basic_native_set */
  { "Ilama",      DT_MBCHARTBL, &VarIlama,      IP "ilama",   NULL              },
  { "Jackfruit",  DT_MBCHARTBL, &VarJackfruit,  0,            NULL              }, /* test_basic_native_get */
  { "Kumquat",    DT_MBCHARTBL, &VarKumquat,    IP "kumquat", NULL              }, /* test_reset */
  { "Lemon",      DT_MBCHARTBL, &VarLemon,      IP "lemon",   validator_succeed }, /* test_validator */
  { "Mango",      DT_MBCHARTBL, &VarMango,      IP "mango",   validator_fail    },
  { "Nectarine",  DT_MBCHARTBL, &VarNectarine,  0,            NULL              }, /* test_inherit */
  { NULL },
};

static bool test_initial_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  printf("Apple = %s\n", VarApple->orig_str);
  printf("Banana = %s\n", VarBanana->orig_str);

  return ((mutt_strcmp(VarApple->orig_str, "apple") == 0) &&
          (mutt_strcmp(VarBanana->orig_str, "banana") == 0));
}

static bool test_basic_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "hello", "world", NULL };
  char *name = "Cherry";
  char *mb = NULL;

  for (int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    if (!cs_set_variable(cs, name, valid[i], err))
    {
      printf("%s\n", err->data);
      return false;
    }

    mb = VarCherry ? VarCherry->orig_str : NULL;
    if (mutt_strcmp(mb, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(mb), NONULL(valid[i]));
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

    if (mutt_strcmp(VarDamson->orig_str, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(VarDamson->orig_str),
           NONULL(valid[i]));
  }

  return true;
}

static bool test_basic_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Elderberry";
  char *mb = NULL;

  mutt_buffer_reset(err);
  if (!cs_get_variable(cs, name, err))
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  mb = VarElderberry ? VarElderberry->orig_str : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(mb), err->data);

  name = "Fig";
  mutt_buffer_reset(err);
  if (!cs_get_variable(cs, name, err))
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  mb = VarFig ? VarFig->orig_str : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(mb), err->data);

  name = "Guava";
  if (!cs_set_variable(cs, name, "guava", err))
    return false;

  mutt_buffer_reset(err);
  if (!cs_get_variable(cs, name, err))
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  mb = VarGuava ? VarGuava->orig_str : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(mb), err->data);

  return true;
}

static bool test_basic_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  struct MbCharTable a = { "hello" };
  char *name = "Hawthorn";
  char *mb = NULL;

  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, (intptr_t) &a, err))
  {
    printf("%s\n", err->data);
    return false;
  }

  mb = VarHawthorn ? VarHawthorn->orig_str : NULL;
  if (mutt_strcmp(mb, a.orig_str) != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }
  printf("%s = '%s', set by '%s'\n", name, NONULL(mb), a.orig_str);

  name = "Ilama";
  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, 0, err))
  {
    printf("%s\n", err->data);
    return false;
  }

  if (VarIlama != NULL)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }
  mb = VarIlama ? VarIlama->orig_str : NULL;
  printf("%s = '%s', set by NULL\n", name, NONULL(mb));

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
  struct MbCharTable *a = (struct MbCharTable *) value;

  if (VarJackfruit != a)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  char *mb1 = VarJackfruit ? VarJackfruit->orig_str : NULL;
  char *mb2 = a ? a->orig_str : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(mb1), NONULL(mb2));

  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Kumquat";
  char *mb = NULL;

  mutt_buffer_reset(err);

  mb = VarKumquat ? VarKumquat->orig_str : NULL;
  printf("Initial: %s = '%s'\n", name, NONULL(mb));
  if (!cs_set_variable(cs, name, "hello", err))
    return false;
  mb = VarKumquat ? VarKumquat->orig_str : NULL;
  printf("Set: %s = '%s'\n", name, NONULL(mb));

  if (!cs_reset_variable(cs, name, err))
  {
    printf("%s\n", err->data);
    return false;
  }

  mb = VarKumquat ? VarKumquat->orig_str : NULL;
  if (mutt_strcmp(mb, "kumquat") != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("Reset: %s = '%s'\n", name, NONULL(mb));

  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Lemon";
  char *mb = NULL;
  struct MbCharTable a = { "world" };

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
  mb = VarLemon ? VarLemon->orig_str : NULL;
  printf("MbCharTable: %s = %s\n", name, NONULL(mb));

  mutt_buffer_reset(err);
  if (cs_str_set_value(cs, name, IP & a, err))
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  mb = VarLemon ? VarLemon->orig_str : NULL;
  printf("Native: %s = %s\n", name, NONULL(mb));

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
  mb = VarMango ? VarMango->orig_str : NULL;
  printf("MbCharTable: %s = %s\n", name, NONULL(mb));

  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, IP & a, err))
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  mb = VarMango ? VarMango->orig_str : NULL;
  printf("Native: %s = %s\n", name, NONULL(mb));

  return true;
}

static void dump_native(struct ConfigSet *cs, const char *parent, const char *child)
{
  intptr_t pval = cs_str_get_value(cs, parent, NULL);
  intptr_t cval = cs_str_get_value(cs, child, NULL);

  struct MbCharTable *pa = (struct MbCharTable *) pval;
  struct MbCharTable *ca = (struct MbCharTable *) cval;

  char *pstr = pa ? pa->orig_str : NULL;
  char *cstr = ca ? ca->orig_str : NULL;

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

  const char *AccountVarMb[] = {
    parent, NULL,
  };

  struct Account *ac = ac_create(cs, account, AccountVarMb);

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

bool mbytetable_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new_set(30);

  init_mbyte_table(cs);
  if (!cs_register_variables(cs, MbCharTableVars))
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
