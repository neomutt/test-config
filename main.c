#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "address.h"
#include "config_set.h"
#include "mutt_regex.h"
#include "mbyte_table.h"
#include "data.h"
#include "lib.h"

unsigned int SOMEPRIME = 149711;

bool listener(struct ConfigSet *set, const char *name, enum ConfigEvent e)
{
  if (e == CE_CHANGED)
    printf("\033[1;33m%s has been changed\033[m\n", name);
  else
    printf("\033[1;32m%s has been set\033[m\n", name);
  return false;
}

bool destructor(struct ConfigSet *set, int type, intptr_t obj)
{
  if (DTYPE(type) != DT_STR)
    return false;

  const char *str = (const char*) obj;
  printf("\033[1;35mFreeing string '%s'\033[m\n", str);
  FREE(&str);
  return true;
}

bool validator(struct ConfigSet *set, const char *name, int type, intptr_t value)
{
  if (type != DT_NUM)
    return true;

  int copy = value;
  if (copy < 250)
  {
    printf("\033[1;36mvalidate: %s = %d (OK)\033[m\n", name, copy);
    return true;
  }
  else
  {
    printf("\033[1;31mvalidate: %s = %d (BAD)\033[m\n", name, copy);
    return false;
  }
}

unsigned int gen_string_hash(const char *key, unsigned int n)
{
  unsigned int h = 0;

  while (*key)
    h += (h << 7) + *key++;
  h = (h * SOMEPRIME) % n;

  return h;
}

void test1(void)
{
  struct ConfigSet cs;
  cs_init(&cs, NULL);
  cs_add_listener(&cs, listener);

  /* set two values, overwrite the second one */

  struct Address *a1 = calloc(1, sizeof(*a1));
  a1->personal = strdup("Richard Example");
  a1->mailbox  = strdup("rich@example.com");
  struct Address *a2 = calloc(2, sizeof(*a2));
  a2->personal = strdup("Dave Metaphor");
  a2->mailbox  = strdup("dave@metaphor.com");
  struct Address *a3 = calloc(3, sizeof(*a3));
  a3->personal = strdup("Wendy Simile");
  a3->mailbox  = strdup("wendy@simile.com");
  cs_set_addr(&cs, a, a1);
  cs_set_addr(&cs, b, a2);
  cs_set_addr(&cs, b, a3);
  printf("DT_ADDR\n");
  printf("    %-10s = %s\n", a, cs_get_addr(&cs, a)->personal);
  printf("    %-10s = %s\n", b, cs_get_addr(&cs, b)->personal);

  cs_set_bool(&cs, c, false);
  cs_set_bool(&cs, d, false);
  cs_set_bool(&cs, d, true);
  printf("DT_BOOL\n");
  printf("    %-10s = %d\n", c, cs_get_bool(&cs, c));
  printf("    %-10s = %d\n", d, cs_get_bool(&cs, d));

  cs_set_hcache(&cs, e, strdup("lmdb"));
  cs_set_hcache(&cs, f, strdup("qdbm"));
  cs_set_hcache(&cs, f, strdup("gdbm"));
  printf("DT_HCACHE\n");
  printf("    %-10s = %s\n", e, cs_get_hcache(&cs, e));
  printf("    %-10s = %s\n", f, cs_get_hcache(&cs, f));

  cs_set_magic(&cs, g, 123);
  cs_set_magic(&cs, h, 456);
  cs_set_magic(&cs, h, 789);
  printf("DT_MAGIC\n");
  printf("    %-10s = %d\n", g, cs_get_magic(&cs, g));
  printf("    %-10s = %d\n", h, cs_get_magic(&cs, h));

  struct MbCharTable *m1 = calloc(1, sizeof(*m1));
  m1->segmented_str = strdup("x y z");
  m1->orig_str      = strdup("xyz");
  struct MbCharTable *m2 = calloc(2, sizeof(*m2));
  m2->segmented_str = strdup("i j g");
  m2->orig_str  = strdup("abc");
  struct MbCharTable *m3 = calloc(3, sizeof(*m3));
  m3->segmented_str = strdup("v a b");
  m3->orig_str  = strdup("pqr");
  cs_set_mbchartbl(&cs, i, m1);
  cs_set_mbchartbl(&cs, j, m2);
  cs_set_mbchartbl(&cs, j, m3);
  printf("DT_MBCHARTBL\n");
  printf("    %-10s = %s\n", i, cs_get_mbchartbl(&cs, i)->segmented_str);
  printf("    %-10s = %s\n", j, cs_get_mbchartbl(&cs, j)->segmented_str);

  cs_set_num(&cs, k, 111);
  cs_set_num(&cs, l, 222);
  cs_set_num(&cs, l, 333);
  printf("DT_NUM\n");
  printf("    %-10s = %d\n", k, cs_get_num(&cs, k));
  printf("    %-10s = %d\n", l, cs_get_num(&cs, l));

  cs_set_path(&cs, m, strdup("/usr"));
  cs_set_path(&cs, n, strdup("/home"));
  cs_set_path(&cs, n, strdup("/etc"));
  printf("DT_PATH\n");
  printf("    %-10s = %s\n", m, cs_get_path(&cs, m));
  printf("    %-10s = %s\n", n, cs_get_path(&cs, n));

  cs_set_quad(&cs, o, 1);
  cs_set_quad(&cs, p, 2);
  cs_set_quad(&cs, p, 3);
  printf("DT_QUAD\n");
  printf("    %-10s = %d\n", o, cs_get_quad(&cs, o));
  printf("    %-10s = %d\n", p, cs_get_quad(&cs, p));

  struct Regex *r1 = calloc(1, sizeof(*r1));
  r1->pattern = strdup("name.*");
  struct Regex *r2 = calloc(2, sizeof(*r2));
  r2->pattern = strdup(".*name");
  struct Regex *r3 = calloc(3, sizeof(*r3));
  r3->pattern = strdup("na.*me");
  cs_set_rx(&cs, q, r1);
  cs_set_rx(&cs, r, r2);
  cs_set_rx(&cs, r, r3);
  printf("DT_RX\n");
  printf("    %-10s = %s\n", q, cs_get_rx(&cs, q)->pattern);
  printf("    %-10s = %s\n", r, cs_get_rx(&cs, r)->pattern);

  cs_set_sort(&cs, s, 2);
  cs_set_sort(&cs, t, 3);
  cs_set_sort(&cs, t, 4);
  printf("DT_SORT\n");
  printf("    %-10s = %d\n", s, cs_get_sort(&cs, s));
  printf("    %-10s = %d\n", t, cs_get_sort(&cs, t));

  cs_set_str(&cs, u, strdup(a));
  cs_set_str(&cs, v, strdup(b));
  cs_set_str(&cs, v, strdup(c));
  printf("DT_STR\n");
  printf("    %-10s = %s\n", u, cs_get_str(&cs, u));
  printf("    %-10s = %s\n", v, cs_get_str(&cs, v));

  cs_free(&cs);
}

void test2(void)
{
  const char *empty = NULL;
  struct ConfigSet parent;
  cs_init(&parent, NULL);
  cs_add_listener(&parent, listener);
  cs_add_destructor(&parent, destructor);

  struct ConfigSet child;
  cs_init(&child, &parent);
  cs_add_listener(&child, listener);
  cs_add_destructor(&child, destructor);

  printf("MISSING\n");
  printf("    PARENT %-10s = %s\n", empty, cs_get_str(&parent, a));
  printf("    CHILD  %-10s = %s\n", empty, cs_get_str(&child,  a));

  cs_set_str(&parent, b, strdup(b));

  printf("PARENT ONLY\n");
  printf("    PARENT %-10s = %s\n", b,     cs_get_str(&parent, b));
  printf("    CHILD  %-10s = %s\n", empty, cs_get_str(&child,  b));

  cs_set_str(&child, c, strdup(c));

  printf("CHILD ONLY\n");
  printf("    PARENT %-10s = %s\n", empty, cs_get_str(&parent, c));
  printf("    CHILD  %-10s = %s\n", c,     cs_get_str(&child,  c));

  cs_set_str(&parent, d, strdup(d));
  cs_set_str(&child,  d, strdup(e));

  printf("OVERRIDE\n");
  printf("    PARENT %-10s = %s\n", d, cs_get_str(&parent, d));
  printf("    CHILD  %-10s = %s\n", e, cs_get_str(&child,  d));

  cs_free(&parent);
  cs_free(&child);
}

void test3(void)
{
  fprintf(stderr, "Prime: %d\n", SOMEPRIME);
  // fprintf(stderr, "%d\n", SOMEPRIME);
  for (int i = 0; MuttVars[i].option; i++)
  {
    printf("%3d %s\n", gen_string_hash(MuttVars[i].option, 500), MuttVars[i].option);
    // printf("%3d\n", gen_string_hash(MuttVars[i].option, 503));
  }
}

void test4(void)
{
  struct ConfigSet parent;
  cs_init(&parent, NULL);
  cs_add_listener(&parent, listener);

  struct ConfigSet child;
  cs_init(&child, &parent);
  cs_add_listener(&child, listener);

  cs_set_str(&parent, d, strdup(d));
  cs_set_str(&child,  d, strdup(e));

  struct HashElem *hep = cs_get_elem(&parent, d);
  struct HashElem *hec = cs_get_elem(&child,  d);

  printf("OVERRIDE\n");
  printf("    PARENT %-10s = %s\n", d, he_get_str(hep));
  printf("    CHILD  %-10s = %s\n", e, he_get_str(hec));

  cs_free(&parent);
  cs_free(&child);
}

void test5(void)
{
  struct ConfigSet cs;
  cs_init(&cs, NULL);
  cs_add_listener(&cs, listener);
  cs_add_validator(&cs, validator);

  cs_set_num(&cs, k, 111);
  cs_set_num(&cs, l, 222);
  cs_set_num(&cs, l, 333);
  printf("DT_NUM\n");
  printf("    %-10s = %d\n", k, cs_get_num(&cs, k));
  printf("    %-10s = %d\n", l, cs_get_num(&cs, l));

  cs_free(&cs);
}


int main(int argc, char *argv[])
{
  // test1();
  test2();
  // if (argc > 1)
  //   SOMEPRIME = atol(argv[1]);
  // test3();
  // test4();
  // test5();
  return 0;
}
