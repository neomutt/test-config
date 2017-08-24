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
#include "type/string3.h"

static char *VarApple;
static char *VarBanana;
static char *VarCherry;

struct VariableDef InitialVars[] = {
  { "Apple",  DT_STR, &VarApple,  IP "apple", NULL },
  { "Banana", DT_STR, &VarBanana, 0,          NULL },
  { "Cherry", DT_STR, &VarCherry, 0,          NULL },
  { NULL },
};

static bool test_set_initial(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);

  bool result = false;

  struct HashElem *he_a = cs_get_elem(cs, "Apple");
  if (!he_a)
    return false;

  const char *aval = safe_strdup("pie");
  if (!cs_set_initial_value(cs, he_a, aval, err))
    printf("Expected error: %s\n", err->data);

  struct HashElem *he_b = cs_get_elem(cs, "Banana");
  if (!he_b)
    return false;

  const char *bval = safe_strdup("split");
  if (!cs_set_initial_value(cs, he_b, bval, err))
    goto tsi_out;
  bval = NULL;

  struct HashElem *he_c = cs_get_elem(cs, "Cherry");
  if (!he_c)
    return false;

  const char *cval = safe_strdup("blossom");
  if (!cs_set_initial_value(cs, he_c, cval, err))
    goto tsi_out;
  cval = NULL;

  result = ((mutt_strcmp(VarApple, aval) != 0) &&
            (mutt_strcmp(VarBanana, bval) == 0));

  printf("Apple = %s\n", VarApple);
  printf("Banana = %s\n", VarBanana);
  printf("Cherry = %s\n", VarCherry);

  // if (!cs_reset_variable(cs, "Apple", err))
  // {
  //   printf("%s\n", err->data);
  //   goto tsi_out;
  // }

  // if (!cs_reset_variable(cs, "Banana", err))
  // {
  //   printf("%s\n", err->data);
  //   goto tsi_out;
  // }

  if (!cs_set_variable(cs, "Cherry", "picker", err))
  {
    printf("%s\n", err->data);
    goto tsi_out;
  }

  printf("Apple = %s\n", VarApple);
  printf("Banana = %s\n", VarBanana);
  printf("Cherry = %s\n", VarCherry);

tsi_out:
  FREE(&aval);
  FREE(&bval);
  FREE(&cval);
  return result;
}

bool initial_test(void)
{
  log_line(__func__);

  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = safe_calloc(1, STRING);
  err.dsize = STRING;
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new_set(30);

  init_string(cs);
  if (!cs_register_variables(cs, InitialVars))
    return false;

  cs_add_listener(cs, log_listener);

  set_list(cs);

  if (!test_set_initial(cs, &err))
  {
    cs_free(&cs);
    FREE(&err.data);
    return false;
  }

  cs_free(&cs);
  FREE(&err.data);

  return true;
}
