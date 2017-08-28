#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "config/account.h"
#include "config/config_set.h"
#include "config/number.h"
#include "config/types.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "mutt_options.h"
#include "test/common.h"

static short VarApple;
static short VarBanana;
static short VarCherry;

// clang-format off
static struct VariableDef Vars[] = {
  { "Apple",  DT_NUM, &VarApple,  0, NULL },
  { "Banana", DT_NUM, &VarBanana, 0, NULL },
  { "Cherry", DT_NUM, &VarCherry, 0, NULL },
  { NULL },
};
// clang-format on

bool account_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new_set(30);

  init_num(cs);
  if (!cs_register_variables(cs, Vars))
    return false;

  set_list(cs);

  cs_add_listener(cs, log_listener);

  const char *account = "fruit";
  const char *BrokenVarStr[] = {
    "Pineapple", NULL,
  };

  struct Account *ac = ac_create(cs, account, BrokenVarStr);
  if (!ac)
  {
    printf("Expected error:\n");
  }
  else
  {
    printf("This test should have failed\n");
    return false;
  }

  const char *AccountVarStr[] = {
    "Apple", "Cherry", NULL,
  };

  ac = ac_create(cs, account, AccountVarStr);
  if (!ac)
    return false;

  int index = 0;
  mutt_buffer_reset(&err);
  if (!ac_set_value(ac, index, 42, &err))
  {
    printf("%s\n", err.data);
  }

  mutt_buffer_reset(&err);
  if (!ac_set_value(ac, 99, 42, &err))
  {
    printf("Expected error: %s\n", err.data);
  }
  else
  {
    printf("This test should have failed\n");
    return false;
  }

  mutt_buffer_reset(&err);
  if (!ac_get_value(ac, index, &err))
  {
    printf("%s\n", err.data);
  }
  else
  {
    printf("%s = %s\n", AccountVarStr[index], err.data);
  }

  ac_free(cs, &ac);
  cs_free(&cs);
  FREE(&err.data);

  return true;
}
