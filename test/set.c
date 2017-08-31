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
  { "Apple",  DT_NUMBER,  &VarApple,  0, NULL },
  { "Banana", DT_BOOL,    &VarBanana, 1, NULL },
  { NULL },
};
// clang-format on

bool set_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_create(30);
  if (!cs)
    return false;

  cs_add_listener(cs, log_listener);
  cs_remove_listener(cs, log_listener);

  const struct ConfigSetType cst_dummy = {
    "dummy", NULL, NULL, NULL, NULL, NULL, NULL,
  };

  if (!cs_register_type(cs, DT_STRING, &cst_dummy))
  {
    printf("Expected error\n");
  }
  else
  {
    printf("This test should have failed\n");
    return false;
  }

  bool_init(cs);
  bool_init(cs); /* second one should fail */

  if (!cs_register_variables(cs, Vars))
  {
    printf("Expected error\n");
  }
  else
  {
    printf("This test should have failed\n");
    return false;
  }

  const char *name = "Unknown";
  int result = cs_str_string_set(cs, name, "hello", &err);
  if ((result & CSR_RESULT_MASK) == CSR_ERR_UNKNOWN)
  {
    printf("Expected error: Unknown var '%s'\n", name);
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