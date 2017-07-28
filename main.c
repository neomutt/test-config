#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "config_set.h"
#include "data.h"
#include "hcache/hcache.h"
#include "imap/imap.h"
#include "lib.h"
#include "ncrypt/ncrypt.h"
#include "nntp.h"
#include "notmuch.h"
#include "pop.h"
#include "sidebar.h"
#include "type/address.h"
#include "type/bool.h"
#include "type/magic.h"
#include "type/mbyte_table.h"
#include "type/mutt_regex.h"
#include "type/number.h"
#include "type/path.h"
#include "type/quad.h"
#include "type/sort.h"
#include "type/string.h"

bool init_types(struct ConfigSet *set)
{
  init_addr();
  init_bool();
  init_magic();
  init_mbyte_table();
  init_regex();
  init_num();
  init_path();
  init_quad();
  init_sorts();
  init_string();

  cs_register_variables(set, MuttVars);

  return true;
}

bool listener(struct ConfigSet *set, const char *name, enum ConfigEvent e)
{
  if (e == CE_CHANGED)
    printf("\033[1;33m%s has been changed\033[m\n", name);
  else
    printf("\033[1;32m%s has been set\033[m\n", name);
  return false;
}

int main(int argc, char *argv[])
{
  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = calloc(1, STRING);
  err.dsize = STRING;

  struct ConfigSet cs;
  cs_init(&cs, NULL);
  cs_add_listener(&cs, listener);

  init_types(&cs);
  init_sorts();

  init_hcache(&cs);
  init_imap(&cs);
  init_ncrypt(&cs);
  init_nntp(&cs);
  init_notmuch(&cs);
  init_pop(&cs);
  init_sidebar(&cs);

  cs_dump_set(&cs);
  cs_free(&cs);
  FREE(&err.data);
  return 0;
}
