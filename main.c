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

const char *str_parent = "resume_draft_files";
const char *str_child  = "wibble:resume_draft_files";

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
  if (e == CE_CHANGED)
    printf("\033[1;33m%s has been changed\033[m\n", name);
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

void dump1(struct ConfigSet *cs)
{
  struct Buffer result;
  mutt_buffer_init(&result);
  result.data = calloc(1, STRING);
  result.dsize = STRING;

  mutt_buffer_reset(&result);
  if (!cs_get_variable(cs, str_parent, &result))
  {
    printf("Error: %s\n", result.data);
    return;
  }
  // printf("%25s = %d\n", str_parent, OPT_RESUME_DRAFT_FILES);
  printf("%25s = %s\n", str_parent, result.data);                               // print value of        resume_draft_files | 0

  mutt_buffer_reset(&result);
  if (!cs_get_variable(cs, str_child, &result))                                 
  {
    printf("Error: %s\n", result.data);
    return;
  }
  printf("%25s = %s\n", str_child, result.data);                                // print value of wibble:resume_draft_files | 0 (inherited)

  FREE(&result.data);
}

void set1(struct ConfigSet *cs, const char *name, const char *value)
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

void test1(struct ConfigSet *cs, struct Account *ac)
{
  // struct HashElem *he_parent = cs_get_elem(cs, str_parent);
  // struct HashElem *he_child  = cs_get_elem(cs, str_child);

  // print value of        resume_draft_files | 0
  // print value of wibble:resume_draft_files | 0 (inherited)
  dump1(cs);

  // set   value of        resume_draft_files = 1
  set1(cs, str_parent, "1");

  // print value of        resume_draft_files | 1
  // print value of wibble:resume_draft_files | 1 (inherited)
  dump1(cs);

  // set   value of wibble:resume_draft_files = 0
  set1(cs, str_child, "0");

  // print value of        resume_draft_files | 1
  // print value of wibble:resume_draft_files | 0 (private)
  dump1(cs);

  // reset value of wibble:resume_draft_files = 1

  // print value of        resume_draft_files | 1
  // print value of wibble:resume_draft_files | 1 (inherited)

  // reset value of        resume_draft_files = 0

  // print value of        resume_draft_files | 0
  // print value of wibble:resume_draft_files | 0 (inherited)
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

  struct Account *ac = account_create("wibble", &cs);
  // printf("ac = %p\n", (void *) ac);

  // cs_dump_set(&cs);
  // hash_dump(cs.hash);

  test1(&cs, ac);

  account_free(&ac);
  cs_free(&cs);
  FREE(&err.data);
  return 0;
}

