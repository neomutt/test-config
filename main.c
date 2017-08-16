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
#include "type/string3.h"

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
  init_hcache(set);
  init_imap(set);
  init_ncrypt(set);
  init_nntp(set);
  init_notmuch(set);
  init_pop(set);
  init_sidebar(set);
}

bool listener(struct ConfigSet *set, struct HashElem *he, const char *name, enum ConfigEvent e)
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

  printf("%26s = %s\n", parent, result.data);

  mutt_buffer_reset(&result);
  if (!cs_get_variable(cs, child, &result))
  {
    printf("Error: %s\n", result.data);
    return;
  }
  printf("%26s = %s\n", child, result.data);

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
    printf("Error: %s\n", result.data);

  FREE(&result.data);
}

void test(struct ConfigSet *cs, const char *account, const char *parent, const char *pvalue, const char *cvalue)
{
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  dump(cs, parent, child);
  set(cs, parent, pvalue);
  dump(cs, parent, child);
  set(cs, child, cvalue);
  dump(cs, parent, child);
  reset(cs, child);
  dump(cs, parent, child);
  reset(cs, parent);
  dump(cs, parent, child);
  printf("\n");
}

void test_set_reset(struct ConfigSet *cs)
{
  struct Account *ac1 = account_create(cs, "apple");
  struct Account *ac2 = account_create(cs, "banana");
  // printf("ac = %p\n", (void *) ac);

  test(cs, "apple",  "attribution",          "date %d",   "from %n");  // DT_STR
  test(cs, "apple",  "resume_draft_files",   "1",         "0");        // DT_BOOL
  test(cs, "apple",  "from",                 "a@b.com",   "x@y.org");  // DT_ADDR
  test(cs, "apple",  "mbox_type",            "mh",        "Maildir");  // DT_MAGIC
  test(cs, "banana", "pager_context",        "12",        "9");        // DT_NUM
  test(cs, "banana", "sort",                 "threads",   "score");    // DT_SORT
  test(cs, "banana", "post_moderated",       "ask-no",    "yes");      // DT_QUAD
  test(cs, "apple",  "alias_file",           "~/a",       "/etc/b");   // DT_PATH
  test(cs, "apple",  "header_cache_backend", "lmdb",      "qdbm");     // DT_HCACHE
  test(cs, "banana", "quote_regexp",         ">.*",       "#.*");      // DT_RX
  test(cs, "banana", "status_chars",         "ABCD",      "prqs");     // DT_MBCHARTBL

  account_free(cs, &ac2);
  account_free(cs, &ac1);
}

void test_validators(struct ConfigSet *cs)
{
  /* failing tests */
  set(cs, "certificate_file",      "file");               // DT_PATH
  set(cs, "alias_format",          "format");             // DT_STR
  set(cs, "envelope_from_address", "jim(at)example.com"); // DT_ADDR
  set(cs, "mbox_type",             "mbox");               // DT_MAGIC
  set(cs, "connect_timeout",       "99");                 // DT_NUM
  set(cs, "abort_noattach",        "ask-no");             // DT_QUAD
  set(cs, "attach_keyword",        "attach.*");           // DT_RX
  set(cs, "allow_8bit",            "yes");                // DT_BOOL
  set(cs, "sort_aux",              "date");               // DT_SORT
  set(cs, "flag_chars",            "ABcd");               // DT_MBCHARTBL

  /* succeeding tests */
  set(cs, "certificate_file",      "dir");                // DT_PATH
  set(cs, "alias_format",          "alias");              // DT_STR
  set(cs, "envelope_from_address", "jim@example.com");    // DT_ADDR
  set(cs, "mbox_type",             "maildir");            // DT_MAGIC
  set(cs, "connect_timeout",       "32");                 // DT_NUM
  set(cs, "abort_noattach",        "ask-yes");            // DT_QUAD
  set(cs, "attach_keyword",        ".*ment");             // DT_RX
  set(cs, "allow_8bit",            "yes");                // DT_BOOL
  set(cs, "sort_aux",              "spam");               // DT_SORT
  set(cs, "flag_chars",            "pQrS");               // DT_MBCHARTBL
}

void test_native(struct ConfigSet *cs)
{
  // struct Buffer result;
  // mutt_buffer_init(&result);
  // result.data = calloc(1, STRING);
  // result.dsize = STRING;

  struct Account *ac = account_create(cs, "cherry");
  bool rdf;

  rdf = get_he_bool(ac, V_RESUME_DRAFT_FILES);
  printf("resume_draft_files = %d\n", rdf);

  // printf("resume_draft_files = %d\n", OPT_RESUME_DRAFT_FILES);
  set_he_bool(ac, V_RESUME_DRAFT_FILES, true);              // DT_BOOL

  rdf = get_he_bool(ac, V_RESUME_DRAFT_FILES);
  printf("resume_draft_files = %d\n", rdf);

  // set_he_addr     (ac, V_FROM,                 "jim@example.com"); // DT_ADDR
  // set_he_hcache   (ac, V_HEADER_CACHE_BACKEND, "lmdb");            // DT_HCACHE
  // set_he_magic    (ac, V_MBOX_TYPE,            "maildir");         // DT_MAGIC
  // set_he_mbchartbl(ac, V_STATUS_CHARS,         "ABCD");            // DT_MBCHARTBL
  // set_he_num      (ac, V_PAGER_CONTEXT,        12);                // DT_NUM
  // set_he_path     (ac, V_ALIAS_FILE,           "/home");           // DT_PATH
  // set_he_quad     (ac, V_POST_MODERATED,       MUTT_ASKYES);       // DT_QUAD
  // set_he_rx       (ac, V_QUOTE_REGEXP,         rx);                // DT_RX
  // set_he_sort     (ac, V_SORT,                 SORT_SPAM);         // DT_SORT
  // set_he_str      (ac, V_ATTRIBUTION,          "flatcap");         // DT_STR

  account_free(cs, &ac);
  // FREE(&result.data);
}

int main(int argc, char *argv[])
{
  struct Buffer err;
  mutt_buffer_init(&err);
  err.data = calloc(1, STRING);
  err.dsize = STRING;

  struct ConfigSet cs;
  cs_init(&cs, NULL, 30);
  cs_add_listener(&cs, listener);

  init_types();
  init_variables(&cs);

  mutt_buffer_reset(&err);

  // cs_dump_set(&cs);
  // hash_dump(cs.hash);
  // test_set_reset(&cs);
  // test_validators(&cs);
  test_native(&cs);

  // printf("header_cache_pagesize = %s\n", HeaderCachePageSize);
  // if (!cs_set_variable(&cs, "header_cache_pagesize", "32768", &err))
  //   printf("Set failed: %s\n", err.data);
  // printf("header_cache_pagesize = %s\n", HeaderCachePageSize);

  cs_free(&cs);
  FREE(&err.data);
  return 0;
}

