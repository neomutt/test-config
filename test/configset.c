#include "config.h"
#include <stdbool.h>
#include "config/bool.h"
#include "config/types.h"
#include "lib/buffer.h"
#include "lib/memory.h"
#include "lib/string2.h"
#include "mutt_options.h"
#include "test/common.h"

static short VarApple;
static bool VarBanana;

// clang-format off
static struct VariableDef Vars[] = {
  { "Apple",  DT_NUM,  &VarApple,  0, NULL },
  { "Banana", DT_BOOL, &VarBanana, 1, NULL },
  { NULL },
};
// clang-format on

bool configset_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new_set(30);

  const struct ConfigSetType cst_dummy = {
    "dummy", NULL, NULL, NULL, NULL, NULL, NULL,
  };
  if (!cs_register_type(cs, DT_STR, &cst_dummy))
  {
    printf("Expected error\n");
  }
  else
  {
    printf("This test should have failed\n");
    return false;
  }

  init_bool(cs);
  init_bool(cs);

  if (!cs_register_variables(cs, Vars))
  {
    printf("Expected error\n");
  }
  else
  {
    printf("This test should have failed\n");
    return false;
  }

  if (!cs_set_variable(cs, "Unknown", "hello", &err))
  {
    printf("Expected error: %s\n", err.data);
  }
  else
  {
    printf("This should have failed\n");
    return false;
  }

  set_list(cs);

  cs_free(&cs);
  FREE(&err.data);

  return true;
}
