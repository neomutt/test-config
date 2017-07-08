#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "address.h"
#include "config_set.h"
#include "mutt_regex.h"
#include "mbyte_table.h"
#include "data.h"

unsigned int SOMEPRIME = 149711;

bool callback(struct ConfigSet *set, const char *name, enum ConfigEvent e)
{
  if (e == CE_CHANGED)
    printf("\033[1;33m%s has been changed\033[m\n", name);
  else
    printf("\033[1;32m%s has been set\033[m\n", name);
  return false;
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
  config_set_init(&cs, NULL);
  config_set_add_callback(&cs, callback);

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
  config_set_addr(&cs, a, a1);
  config_set_addr(&cs, b, a2);
  config_set_addr(&cs, b, a3);
  printf("DT_ADDR\n");
  printf("    %-10s = %s\n", a, config_get_addr(&cs, a)->personal);
  printf("    %-10s = %s\n", b, config_get_addr(&cs, b)->personal);

  config_set_bool(&cs, c, false);
  config_set_bool(&cs, d, false);
  config_set_bool(&cs, d, true);
  printf("DT_BOOL\n");
  printf("    %-10s = %d\n", c, config_get_bool(&cs, c));
  printf("    %-10s = %d\n", d, config_get_bool(&cs, d));

  config_set_hcache(&cs, e, strdup("lmdb"));
  config_set_hcache(&cs, f, strdup("qdbm"));
  config_set_hcache(&cs, f, strdup("gdbm"));
  printf("DT_HCACHE\n");
  printf("    %-10s = %s\n", e, config_get_hcache(&cs, e));
  printf("    %-10s = %s\n", f, config_get_hcache(&cs, f));

  config_set_magic(&cs, g, 123);
  config_set_magic(&cs, h, 456);
  config_set_magic(&cs, h, 789);
  printf("DT_MAGIC\n");
  printf("    %-10s = %d\n", g, config_get_magic(&cs, g));
  printf("    %-10s = %d\n", h, config_get_magic(&cs, h));

  struct MbCharTable *m1 = calloc(1, sizeof(*m1));
  m1->segmented_str = strdup("x y z");
  m1->orig_str      = strdup("xyz");
  struct MbCharTable *m2 = calloc(2, sizeof(*m2));
  m2->segmented_str = strdup("i j g");
  m2->orig_str  = strdup("abc");
  struct MbCharTable *m3 = calloc(3, sizeof(*m3));
  m3->segmented_str = strdup("v a b");
  m3->orig_str  = strdup("pqr");
  config_set_mbchartbl(&cs, i, m1);
  config_set_mbchartbl(&cs, j, m2);
  config_set_mbchartbl(&cs, j, m3);
  printf("DT_MBCHARTBL\n");
  printf("    %-10s = %s\n", i, config_get_mbchartbl(&cs, i)->segmented_str);
  printf("    %-10s = %s\n", j, config_get_mbchartbl(&cs, j)->segmented_str);

  config_set_num(&cs, k, 111);
  config_set_num(&cs, l, 222);
  config_set_num(&cs, l, 333);
  printf("DT_NUM\n");
  printf("    %-10s = %d\n", k, config_get_num(&cs, k));
  printf("    %-10s = %d\n", l, config_get_num(&cs, l));

  config_set_path(&cs, m, strdup("/usr"));
  config_set_path(&cs, n, strdup("/home"));
  config_set_path(&cs, n, strdup("/etc"));
  printf("DT_PATH\n");
  printf("    %-10s = %s\n", m, config_get_path(&cs, m));
  printf("    %-10s = %s\n", n, config_get_path(&cs, n));

  config_set_quad(&cs, o, 1);
  config_set_quad(&cs, p, 2);
  config_set_quad(&cs, p, 3);
  printf("DT_QUAD\n");
  printf("    %-10s = %d\n", o, config_get_quad(&cs, o));
  printf("    %-10s = %d\n", p, config_get_quad(&cs, p));

  struct Regex *r1 = calloc(1, sizeof(*r1));
  r1->pattern = strdup("name.*");
  struct Regex *r2 = calloc(2, sizeof(*r2));
  r2->pattern = strdup(".*name");
  struct Regex *r3 = calloc(3, sizeof(*r3));
  r3->pattern = strdup("na.*me");
  config_set_rx(&cs, q, r1);
  config_set_rx(&cs, r, r2);
  config_set_rx(&cs, r, r3);
  printf("DT_RX\n");
  printf("    %-10s = %s\n", q, config_get_rx(&cs, q)->pattern);
  printf("    %-10s = %s\n", r, config_get_rx(&cs, r)->pattern);

  config_set_sort(&cs, s, 2);
  config_set_sort(&cs, t, 3);
  config_set_sort(&cs, t, 4);
  printf("DT_SORT\n");
  printf("    %-10s = %d\n", s, config_get_sort(&cs, s));
  printf("    %-10s = %d\n", t, config_get_sort(&cs, t));

  config_set_str(&cs, u, strdup(a));
  config_set_str(&cs, v, strdup(b));
  config_set_str(&cs, v, strdup(c));
  printf("DT_STR\n");
  printf("    %-10s = %s\n", u, config_get_str(&cs, u));
  printf("    %-10s = %s\n", v, config_get_str(&cs, v));

  config_set_free(&cs);
}

void test2(void)
{
  const char *empty = NULL;
  struct ConfigSet parent;
  config_set_init(&parent, NULL);
  config_set_add_callback(&parent, callback);

  struct ConfigSet child;
  config_set_init(&child, &parent);
  config_set_add_callback(&child, callback);

  printf("MISSING\n");
  printf("    PARENT %-10s = %s\n", empty, config_get_str(&parent, a));
  printf("    CHILD  %-10s = %s\n", empty, config_get_str(&child,  a));

  config_set_str(&parent, b, strdup(b));

  printf("PARENT ONLY\n");
  printf("    PARENT %-10s = %s\n", b,     config_get_str(&parent, b));
  printf("    CHILD  %-10s = %s\n", empty, config_get_str(&child,  b));

  config_set_str(&child, c, strdup(c));

  printf("CHILD ONLY\n");
  printf("    PARENT %-10s = %s\n", empty, config_get_str(&parent, c));
  printf("    CHILD  %-10s = %s\n", c,     config_get_str(&child,  c));

  config_set_str(&parent, d, strdup(d));
  config_set_str(&child,  d, strdup(e));

  printf("OVERRIDE\n");
  printf("    PARENT %-10s = %s\n", d, config_get_str(&parent, d));
  printf("    CHILD  %-10s = %s\n", e, config_get_str(&child,  d));

  config_set_free(&parent);
  config_set_free(&child);
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

int main(int argc, char *argv[])
{
  test1();
  test2();
  if (argc > 1)
    SOMEPRIME = atol(argv[1]);
  test3();
  return 0;
}
