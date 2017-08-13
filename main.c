#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "account.h"
#include "config_set.h"
#include "data.h"
#include "hcache/hcache.h"
#include "imap/imap.h"
#include "inheritance.h"
#include "lib/lib.h"
#include "mutt_options.h"
#include "ncrypt/ncrypt.h"
#include "nntp.h"
#include "notmuch.h"
#include "options.h"
#include "pop.h"
#include "sidebar.h"
#include "type/address.h"
#include "type/bool.h"
#include "type/magic.h"
#include "type/mbyte_table.h"
#include "type/number.h"
#include "type/path.h"
#include "type/quad.h"
#include "type/regex.h"
#include "type/sort.h"
#include "type/string.h"

void init_types()
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
}

void init_variables(struct ConfigSet *set)
{
  cs_register_variables(set, MuttVars);
  // init_hcache(set);
  // init_imap(set);
  // init_ncrypt(set);
  // init_nntp(set);
  // init_notmuch(set);
  // init_pop(set);
  // init_sidebar(set);
}

bool listener(struct ConfigSet *set, const char *name, enum ConfigEvent e)
{
  if (e == CE_RESET)
    printf("\033[1;32m%s has been reset\033[m\n", name);
  else
    printf("\033[1;32m%s has been set\033[m\n", name);
  return false;
}

void hash_dump(struct Hash *table)
{
  struct HashElem *he = NULL;

  for (int i = 0; i < table->nelem; i++)
  {
    he = table->table[i];
    if (!he)
      continue;

    printf("%03d ", i);
    for (; he; he = he->next)
    {
      if (he->type & DT_INHERITED)
      {
        struct Inheritance *i = he->data;
        printf("\033[1;32m[%s]\033[m ", i->name);
      }
      else
      {
        printf("%s ", *(char**) he->data);
      }
    }
    printf("\n");
  }
}

void dump(struct ConfigSet *cs, const char *parent, const char *child)
{
  struct Buffer result;
  mutt_buffer_init(&result);
  result.data = calloc(1, STRING);
  result.dsize = STRING;

  mutt_buffer_reset(&result);
  if (!cs_get_variable(cs, parent, &result))
  {
    printf("Error: %s\n", result.data);
    return;
  }

  printf("%30s = %s\n", parent, result.data);

  mutt_buffer_reset(&result);
  if (!cs_get_variable(cs, child, &result))                                 
  {
    printf("Error: %s\n", result.data);
    return;
  }
  printf("%30s = %s\n", child, result.data);

  FREE(&result.data);
}

void reset(struct ConfigSet *cs, const char *name)
{
  struct Buffer result;
  mutt_buffer_init(&result);
  result.data = calloc(1, STRING);
  result.dsize = STRING;

  mutt_buffer_reset(&result);
  if (!cs_reset_variable(cs, name, &result))
  {
    printf("Error: %s\n", result.data);
    return;
  }

  FREE(&result.data);
}

void set(struct ConfigSet *cs, const char *name, const char *value)
{
  struct Buffer result;
  mutt_buffer_init(&result);
  result.data = calloc(1, STRING);
  result.dsize = STRING;

  mutt_buffer_reset(&result);
  if (!cs_set_variable(cs, name, value, &result))
  {
    printf("Error: %s\n", result.data);
    return;
  }

  FREE(&result.data);
}

void test(struct ConfigSet *cs, const char *parent, const char *pvalue, const char *child, const char *cvalue)
{
  // print value of parent | 0
  // print value of child  | 0 (inherited)
  dump(cs, parent, child);

  // set   value of parent = 1
  set(cs, parent, pvalue);

  // print value of parent | 1
  // print value of child  | 1 (inherited)
  dump(cs, parent, child);

  // set   value of child  = 0
  set(cs, child, cvalue);

  // print value of parent | 1
  // print value of child  | 0 (private)
  dump(cs, parent, child);

  // reset value of child  = 1
  reset(cs, child);

  // print value of parent | 1
  // print value of child  | 1 (inherited)
  dump(cs, parent, child);

  // reset value of parent = 0
  reset(cs, parent);

  // print value of parent | 0
  // print value of child  | 0 (inherited)
  dump(cs, parent, child);
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

  init_types();
  init_variables(&cs);

  mutt_buffer_reset(&err);

  // printf("header_cache_pagesize = %s\n", HeaderCachePageSize);
  // if (!cs_set_variable(&cs, "header_cache_pagesize", "32768", &err))
  //   printf("Set failed: %s\n", err.data);
  // printf("header_cache_pagesize = %s\n", HeaderCachePageSize);

  struct Account *ac1 = account_create("wibble",   &cs);
  struct Account *ac2 = account_create("hatstand", &cs);
  // printf("ac = %p\n", (void *) ac);

  // cs_dump_set(&cs);
  // hash_dump(cs.hash);

  test(&cs, "resume_draft_files", "0",  "wibble:resume_draft_files", "1");
  test(&cs, "pager_context",      "12", "hatstand:pager_context",    "9");

  account_free(&ac2);
  account_free(&ac1);
  cs_free(&cs);
  FREE(&err.data);
  return 0;
}

