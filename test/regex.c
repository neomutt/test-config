#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "account.h"
#include "config_set.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "mutt_options.h"
#include "test/common.h"
#include "type/regex2.h"

static struct Regex *VarApple;
static struct Regex *VarBanana;
static struct Regex *VarCherry;
static struct Regex *VarDamson;
static struct Regex *VarElderberry;
static struct Regex *VarFig;
static struct Regex *VarGuava;
static struct Regex *VarHawthorn;
static struct Regex *VarIlama;
static struct Regex *VarJackfruit;
static struct Regex *VarKumquat;
static struct Regex *VarLemon;
static struct Regex *VarMango;
static struct Regex *VarNectarine;

const struct VariableDef RegexVars[] = {
  { "Apple",      DT_RX, &VarApple,      IP "apple.*",   NULL              }, /* test_initial() */
  { "Banana",     DT_RX, &VarBanana,     IP "banana.*",  NULL              },
  { "Cherry",     DT_RX, &VarCherry,     0,              NULL              }, /* test_basic_regex_set */
  { "Damson",     DT_RX, &VarDamson,     IP "damson.*",  NULL              },
  { "Elderberry", DT_RX, &VarElderberry, 0,              NULL              }, /* test_basic_regex_get */
  { "Fig",        DT_RX, &VarFig,        IP "fig.*",     NULL              },
  { "Guava",      DT_RX, &VarGuava,      0,              NULL              },
  { "Hawthorn",   DT_RX, &VarHawthorn,   0,              NULL              }, /* test_basic_native_set */
  { "Ilama",      DT_RX, &VarIlama,      IP "ilama.*",   NULL              },
  { "Jackfruit",  DT_RX, &VarJackfruit,  0,              NULL              }, /* test_basic_native_get */
  { "Kumquat",    DT_RX, &VarKumquat,    IP "kumquat.*", NULL              }, /* test_reset */
  { "Lemon",      DT_RX, &VarLemon,      IP "lemon.*",   validator_succeed }, /* test_validator */
  { "Mango",      DT_RX, &VarMango,      IP "mango.*",   validator_fail    },
  { "Nectarine",  DT_RX, &VarNectarine,  0,              NULL              }, /* test_inherit */
  { NULL },
};

static bool test_initial_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  printf("Apple = %s\n", VarApple->pattern);
  printf("Banana = %s\n", VarBanana->pattern);

  return ((mutt_strcmp(VarApple->pattern, "apple.*") == 0) &&
          (mutt_strcmp(VarBanana->pattern, "banana.*") == 0));
}

static bool test_basic_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *valid[] = { "hello.*", "world.*", NULL };
  char *name = "Cherry";
  char *rx = NULL;

  for (int i = 0; i < mutt_array_size(valid); i++)
  {
    mutt_buffer_reset(err);
    if (!cs_set_variable(cs, name, valid[i], err))
    {
      printf("%s\n", err->data);
      return false;
    }

    rx = VarCherry ? VarCherry->pattern : NULL;
    if (mutt_strcmp(rx, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(rx), NONULL(valid[i]));
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

    rx = VarDamson ? VarDamson->pattern : NULL;
    if (mutt_strcmp(rx, valid[i]) != 0)
    {
      printf("Value of %s wasn't changed\n", name);
      return false;
    }
    printf("%s = '%s', set by '%s'\n", name, NONULL(rx), NONULL(valid[i]));
  }

  return true;
}

static bool test_basic_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Elderberry";
  char *rx = NULL;

  mutt_buffer_reset(err);
  if (!cs_get_variable(cs, name, err))
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  rx = VarElderberry ? VarElderberry->pattern : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(rx), err->data);

  name = "Fig";
  mutt_buffer_reset(err);
  if (!cs_get_variable(cs, name, err))
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  rx = VarFig ? VarFig->pattern : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(rx), err->data);

  name = "Guava";
  if (!cs_set_variable(cs, name, "guava", err))
    return false;

  mutt_buffer_reset(err);
  if (!cs_get_variable(cs, name, err))
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  rx = VarGuava ? VarGuava->pattern : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(rx), err->data);

  return true;
}

static bool test_basic_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  struct Regex r = { "hello.*" };
  char *name = "Hawthorn";
  char *rx = NULL;

  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, (intptr_t) &r, err))
  {
    printf("%s\n", err->data);
    return false;
  }

  rx = VarHawthorn ? VarHawthorn->pattern : NULL;
  if (mutt_strcmp(rx, r.pattern) != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }
  printf("%s = '%s', set by '%s'\n", name, NONULL(rx), r.pattern);

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
  rx = VarIlama ? VarIlama->pattern : NULL;
  printf("%s = '%s', set by NULL\n", name, NONULL(rx));

  return true;
}

static bool test_basic_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Jackfruit";

  if (!cs_set_variable(cs, name, "jackfruit.*", err))
    return false;

  mutt_buffer_reset(err);
  intptr_t value = cs_str_get_value(cs, name, err);
  struct Regex *r = (struct Regex *) value;

  if (VarJackfruit != r)
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  char *rx1 = VarJackfruit ? VarJackfruit->pattern : NULL;
  char *rx2 = r ? r->pattern : NULL;
  printf("%s = '%s', '%s'\n", name, NONULL(rx1), NONULL(rx2));

  return true;
}

static bool test_reset(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Kumquat";
  char *rx = NULL;

  mutt_buffer_reset(err);

  rx = VarKumquat ? VarKumquat->pattern : NULL;
  printf("Initial: %s = '%s'\n", name, NONULL(rx));
  if (!cs_set_variable(cs, name, "hello.*", err))
    return false;
  rx = VarKumquat ? VarKumquat->pattern : NULL;
  printf("Set: %s = '%s'\n", name, NONULL(rx));

  if (!cs_reset_variable(cs, name, err))
  {
    printf("%s\n", err->data);
    return false;
  }

  rx = VarKumquat ? VarKumquat->pattern : NULL;
  if (mutt_strcmp(rx, "kumquat.*") != 0)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("Reset: %s = '%s'\n", name, NONULL(rx));

  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Lemon";
  char *rx = NULL;
  struct Regex r = { "world.*" };

  mutt_buffer_reset(err);
  if (cs_set_variable(cs, name, "hello.*", err))
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  rx = VarLemon ? VarLemon->pattern : NULL;
  printf("Regex: %s = %s\n", name, NONULL(rx));

  mutt_buffer_reset(err);
  if (cs_str_set_value(cs, name, IP &r, err))
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  rx = VarLemon ? VarLemon->pattern : NULL;
  printf("Native: %s = %s\n", name, NONULL(rx));

  name = "Mango";
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, name, "hello.*", err))
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  rx = VarMango ? VarMango->pattern : NULL;
  printf("Regex: %s = %s\n", name, NONULL(rx));

  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, IP &r, err))
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  rx = VarMango ? VarMango->pattern : NULL;
  printf("Native: %s = %s\n", name, NONULL(rx));

  return true;
}

static void dump_native(struct ConfigSet *cs, const char *parent, const char *child)
{
  intptr_t pval = cs_str_get_value(cs, parent, NULL);
  intptr_t cval = cs_str_get_value(cs, child,  NULL);

  struct Regex *pa = (struct Regex *) pval;
  struct Regex *ca = (struct Regex *) cval;

  char *pstr = pa ? pa->pattern : NULL;
  char *cstr = ca ? ca->pattern : NULL;

  printf("%15s = %s\n", parent, NONULL(pstr));
  printf("%15s = %s\n", child,  NONULL(cstr));
}

static bool test_inherit(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  bool result = false;

  const char *account = "fruit";
  const char *parent = "Nectarine";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarRx[] = { parent, NULL, };

  struct Account *ac = ac_create(cs, account,  AccountVarRx);

  // set parent
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, parent, "hello.*", err))
  {
    printf("Error: %s\n", err->data);
    goto bti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, child, "world.*", err))
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

bool regex_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new_set(30);

  init_regex(cs);
  if (!cs_register_variables(cs, RegexVars))
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

