#include "test/acutest.h"

/******************************************************************************
 * Add your test cases to this list.
 *****************************************************************************/
#define NEOMUTT_TEST_LIST                                                      \
  NEOMUTT_TEST_ITEM(config_set)                                                \
  NEOMUTT_TEST_ITEM(config_account)                                            \
  NEOMUTT_TEST_ITEM(config_initial)                                            \
  NEOMUTT_TEST_ITEM(config_synonym)                                            \
  NEOMUTT_TEST_ITEM(config_address)                                            \
  NEOMUTT_TEST_ITEM(config_bool)                                               \
  NEOMUTT_TEST_ITEM(config_command)                                            \
  NEOMUTT_TEST_ITEM(config_long)                                               \
  NEOMUTT_TEST_ITEM(config_magic)                                              \
  NEOMUTT_TEST_ITEM(config_mbtable)                                            \
  NEOMUTT_TEST_ITEM(config_number)                                             \
  NEOMUTT_TEST_ITEM(config_path)                                               \
  NEOMUTT_TEST_ITEM(config_quad)                                               \
  NEOMUTT_TEST_ITEM(config_regex)                                              \
  NEOMUTT_TEST_ITEM(config_sort)                                               \
  NEOMUTT_TEST_ITEM(config_string)

/******************************************************************************
 * You probably don't need to touch what follows.
 *****************************************************************************/
#define NEOMUTT_TEST_ITEM(x) void x(void);
NEOMUTT_TEST_LIST
#undef NEOMUTT_TEST_ITEM

TEST_LIST = {
#define NEOMUTT_TEST_ITEM(x) { #x, x },
  NEOMUTT_TEST_LIST
#undef NEOMUTT_TEST_ITEM
  { 0 }
};
