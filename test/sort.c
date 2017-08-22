#include <stdio.h>
#include <stdbool.h>
#include "type/sort.h"
#include "config_set.h"
#include "debug.h"
#include "mapping.h"
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
static short VarIlama;
static short VarJackfruit;
static short VarKumquat;
static short VarLemon;
static short VarMango;
static short VarNectarine;
static short VarOlive;
static short VarPapaya;

const struct VariableDef SortVars[] = {
  { "Apple",      DT_SORT,                           &VarApple,       1,  NULL              }, /* test_initial() */
  { "Banana",     DT_SORT,                           &VarBanana,      2,  NULL              },
  { "Cherry",     DT_SORT|DT_SORT_INDEX,             &VarCherry,      1,  NULL              }, /* test_basic_string_set */
  { "Damson",     DT_SORT|DT_SORT_ALIAS,             &VarDamson,      11, NULL              },
  { "Elderberry", DT_SORT|DT_SORT_BROWSER,           &VarElderberry,  1,  NULL              },
  { "Fig",        DT_SORT|DT_SORT_KEYS,              &VarFig,         1,  NULL              },
  { "Guava",      DT_SORT|DT_SORT_AUX,               &VarGuava,       1,  NULL              },
  { "Hawthorn",   DT_SORT|DT_SORT_SIDEBAR,           &VarHawthorn,    17, NULL              },
  { "Ilama",      DT_SORT,                           &VarIlama,       1,  NULL              }, /* test_basic_string_get */
  { "Jackfruit",  DT_SORT,                           &VarJackfruit,   1,  NULL              }, /* test_basic_native_set */
  { "Kumquat",    DT_SORT,                           &VarKumquat,     1,  NULL              }, /* test_basic_native_get */
  { "Lemon",      DT_SORT,                           &VarLemon,       1,  NULL              }, /* test_reset */
  { "Mango",      DT_SORT,                           &VarMango,       1,  validator_succeed }, /* test_validator */
  { "Nectarine",  DT_SORT,                           &VarNectarine,   1,  validator_fail    },
  { "Olive",      DT_SORT,                           &VarOlive,       1,  NULL              }, /* test_inherit */
  { "Papaya",     DT_SORT|DT_SORT_AUX|DT_SORT_ALIAS, &VarPapaya,      0,  NULL              }, /* test_sort_type */
  { NULL },
};

static const struct Mapping SortAliasMethods[] = {
  { "alias", SORT_ALIAS }, { "address", SORT_ADDRESS }, { "unsorted", SORT_ORDER }, { NULL, 0 },
};

static const struct Mapping SortAuxMethods[] = {
  { "date", SORT_DATE },
  { "date-sent", SORT_DATE },
  { "date-received", SORT_RECEIVED },
  { "mailbox-order", SORT_ORDER },
  { "subject", SORT_SUBJECT },
  { "from", SORT_FROM },
  { "size", SORT_SIZE },
  { "threads", SORT_DATE },
  { "to", SORT_TO },
  { "score", SORT_SCORE },
  { "spam", SORT_SPAM },
  { "label", SORT_LABEL },
  { NULL, 0 },
};

static const struct Mapping SortBrowserMethods[] = {
  { "alpha", SORT_SUBJECT },  { "count", SORT_COUNT },
  { "date", SORT_DATE },      { "desc", SORT_DESC },
  { "new", SORT_UNREAD },     { "size", SORT_SIZE },
  { "unsorted", SORT_ORDER }, { NULL, 0 },
};

static const struct Mapping SortKeyMethods[] = {
  { "address", SORT_ADDRESS }, { "date", SORT_DATE }, { "keyid", SORT_KEYID },
  { "trust", SORT_TRUST },     { NULL, 0 },
};

static const struct Mapping SortMethods[] = {
  { "date", SORT_DATE },
  { "date-sent", SORT_DATE },
  { "date-received", SORT_RECEIVED },
  { "mailbox-order", SORT_ORDER },
  { "subject", SORT_SUBJECT },
  { "from", SORT_FROM },
  { "size", SORT_SIZE },
  { "threads", SORT_THREADS },
  { "to", SORT_TO },
  { "score", SORT_SCORE },
  { "spam", SORT_SPAM },
  { "label", SORT_LABEL },
  { NULL, 0 },
};

static const struct Mapping SortSidebarMethods[] = {
  { "alpha", SORT_PATH },
  { "count", SORT_COUNT },
  { "desc", SORT_DESC },
  { "flagged", SORT_FLAGGED },
  { "mailbox-order", SORT_ORDER },
  { "name", SORT_PATH },
  { "new", SORT_UNREAD },
  { "path", SORT_PATH },
  { "unread", SORT_UNREAD },
  { "unsorted", SORT_ORDER },
  { NULL, 0 },
};

char *name_list[] = {
  "Cherry", "Damson", "Elderberry", "Fig", "Guava", "Hawthorn",
};

short *var_list[] = {
  &VarCherry, &VarDamson, &VarElderberry, &VarFig, &VarGuava, &VarHawthorn,
};

const struct Mapping *sort_maps[] = {
  SortMethods,    SortAliasMethods, SortBrowserMethods,
  SortKeyMethods, SortAuxMethods,   SortSidebarMethods,
};

static bool test_initial_values(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  printf("Apple = %d\n", VarApple);
  printf("Banana = %d\n", VarBanana);
  return ((VarApple == 1) && (VarBanana == 2));
}

static bool test_basic_string_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  for (int i = 0; i < mutt_array_size(var_list); i++)
  {
    short *var = var_list[i];

    *var = -1;

    const struct Mapping *map = sort_maps[i];

    for (int j = 0; map[j].name; j++)
    {
      mutt_buffer_reset(err);
      if (!cs_set_variable(cs, name_list[i], map[j].name, err))
      {
        printf("%s\n", err->data);
        return false;
      }

      if (*var != map[j].value)
      {
        printf("Value of %s wasn't changed\n", map[j].name);
        return false;
      }
      printf("%s = %d, set by '%s'\n", name_list[i], *var, map[j].name);
    }

    const char *invalid[] = { "-1", "999", "junk", NULL, };

    for (int j = 0; j < mutt_array_size(invalid); j++)
    {
      mutt_buffer_reset(err);
      if (!cs_set_variable(cs, name_list[i], invalid[j], err))
      {
        printf("Expected error: %s\n", err->data);
      }
      else
      {
        printf("%s = %d, set by '%s'\n", name_list[i], *var, invalid[j]);
        printf("This test should have failed\n");
        return false;
      }
    }
  }

  return true;
}

static bool test_basic_string_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Ilama";

  VarIlama = SORT_SUBJECT;
  mutt_buffer_reset(err);
  if (!cs_get_variable(cs, name, err))
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = %d, %s\n", name, VarIlama, err->data);

  VarIlama = SORT_THREADS;
  mutt_buffer_reset(err);
  if (!cs_get_variable(cs, name, err))
  {
    printf("Get failed: %s\n", err->data);
    return false;
  }
  printf("%s = %d, %s\n", name, VarIlama, err->data);

  return true;
}

static bool test_basic_native_set(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Jackfruit";
  short value = SORT_THREADS;

  VarJackfruit = -1;
  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, value, err))
  {
    printf("%s\n", err->data);
    return false;
  }

  if (VarJackfruit != value)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("%s = %d, set to '%d'\n", name, VarJackfruit, value);

  int invalid[] = { -1, 999 };
  for (int i = 0; i < mutt_array_size(invalid); i++)
  {
    VarJackfruit = -1;
    mutt_buffer_reset(err);
    if (!cs_str_set_value(cs, name, invalid[i], err))
    {
      printf("Expected error: %s\n", err->data);
    }
    else
    {
      printf("%s = %d, set by '%d'\n", name, VarJackfruit, invalid[i]);
      printf("This test should have failed\n");
      return false;
    }
  }

  return true;
}

static bool test_basic_native_get(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  char *name = "Kumquat";

  VarKumquat = SORT_THREADS;
  mutt_buffer_reset(err);
  intptr_t value = cs_str_get_value(cs, name, err);
  if (value != SORT_THREADS)
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

  char *name = "Lemon";
  VarLemon = SORT_SUBJECT;
  mutt_buffer_reset(err);

  if (!cs_reset_variable(cs, name, err))
  {
    printf("%s\n", err->data);
    return false;
  }

  if (VarLemon == SORT_SUBJECT)
  {
    printf("Value of %s wasn't changed\n", name);
    return false;
  }

  printf("Reset: %s = %d\n", name, VarLemon);

  return true;
}

static bool test_validator(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  char *name = "Mango";
  VarMango = SORT_SUBJECT;
  mutt_buffer_reset(err);
  if (cs_set_variable(cs, name, "threads", err))
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %d\n", name, VarMango);

  VarMango = SORT_SUBJECT;
  mutt_buffer_reset(err);
  if (cs_str_set_value(cs, name, SORT_THREADS, err))
  {
    printf("%s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("Native: %s = %d\n", name, VarMango);

  name = "Nectarine";
  VarNectarine = SORT_SUBJECT;
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, name, "threads", err))
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("String: %s = %d\n", name, VarNectarine);

  VarNectarine = SORT_SUBJECT;
  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, SORT_THREADS, err))
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s\n", err->data);
    return false;
  }
  printf("Native: %s = %d\n", name, VarNectarine);

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
  const char *parent = "Olive";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  const char *AccountVarStr[] = { parent, NULL, };

  struct Account *ac = ac_create(cs, account,  AccountVarStr);

  // set parent
  VarOlive = SORT_SUBJECT;
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, parent, "threads", err))
  {
    printf("Error: %s\n", err->data);
    goto bti_out;
  }
  dump_native(cs, parent, child);

  // set child
  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, child, "score", err))
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

static bool test_sort_type(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  const char *name = "Papaya";
  char *value = "alpha";

  mutt_buffer_reset(err);
  if (!cs_set_variable(cs, name, value, err))
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s = %d, set by '%s'\n", name, VarPapaya, value);
    printf("This test should have failed\n");
    return false;
  }

  mutt_buffer_reset(err);
  if (!cs_str_set_value(cs, name, SORT_THREADS, err))
  {
    printf("Expected error: %s\n", err->data);
  }
  else
  {
    printf("%s = %d, set by %d\n", name, VarPapaya, SORT_THREADS);
    printf("This test should have failed\n");
    return false;
  }

  return true;
}

bool sort_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new_set(30);

  init_sorts(cs);
  if (!cs_register_variables(cs, SortVars))
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
  if (!test_sort_type(cs, &err))        return false;

  cs_free(&cs);
  FREE(&err.data);

  return true;
}
