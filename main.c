#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "address.h"
#include "config_set.h"
#include "mutt_regex.h"
#include "mbyte_table.h"

const char *a = "apple";
const char *b = "banana";
const char *c = "cherry";
const char *d = "damson";
const char *e = "elderberry";
const char *f = "fig";
const char *g = "guava";
const char *h = "hawthorn";
const char *i = "ilama";
const char *j = "jackfruit";
const char *k = "kumquat";
const char *l = "lemon";
const char *m = "mango";
const char *n = "nectarine";
const char *o = "olive";
const char *p = "papaya";
const char *q = "quince";
const char *r = "raspberry";
const char *s = "strawberry";
const char *t = "tangerine";
const char *u = "ugli";
const char *v = "vanilla";
const char *w = "wolfberry";
const char *x = "xigua";
const char *y = "yew";
const char *z = "ziziphus";

int main()
{
  struct ConfigSet cs;
  config_set_init(&cs);

  /* set two values, overwrite the second one */

  config_set_bool(&cs, a, false);
  config_set_bool(&cs, b, false);
  config_set_bool(&cs, b, true);
  printf("%-10s = %d\n", a, config_get_bool(&cs, a));
  printf("%-10s = %d\n", b, config_get_bool(&cs, b));

  config_set_magic(&cs, c, 123);
  config_set_magic(&cs, d, 456);
  config_set_magic(&cs, d, 789);
  printf("%-10s = %d\n", c, config_get_magic(&cs, c));
  printf("%-10s = %d\n", d, config_get_magic(&cs, d));

  config_set_num(&cs, e, 111);
  config_set_num(&cs, f, 222);
  config_set_num(&cs, f, 333);
  printf("%-10s = %d\n", e, config_get_num(&cs, e));
  printf("%-10s = %d\n", f, config_get_num(&cs, f));

  config_set_quad(&cs, g, 1);
  config_set_quad(&cs, h, 2);
  config_set_quad(&cs, h, 3);
  printf("%-10s = %d\n", g, config_get_quad(&cs, g));
  printf("%-10s = %d\n", h, config_get_quad(&cs, h));

  config_set_sort(&cs, i, 2);
  config_set_sort(&cs, j, 3);
  config_set_sort(&cs, j, 4);
  printf("%-10s = %d\n", i, config_get_sort(&cs, i));
  printf("%-10s = %d\n", j, config_get_sort(&cs, j));

  config_set_hcache(&cs, k, strdup("lmdb"));
  config_set_hcache(&cs, l, strdup("qdbm"));
  config_set_hcache(&cs, l, strdup("gdbm"));
  printf("%-10s = %s\n", k, config_get_hcache(&cs, k));
  printf("%-10s = %s\n", l, config_get_hcache(&cs, l));

  config_set_path(&cs, m, strdup("/usr"));
  config_set_path(&cs, n, strdup("/home"));
  config_set_path(&cs, n, strdup("/etc"));
  printf("%-10s = %s\n", m, config_get_path(&cs, m));
  printf("%-10s = %s\n", n, config_get_path(&cs, n));

  config_set_str(&cs, o, strdup("apple"));
  config_set_str(&cs, p, strdup("banana"));
  config_set_str(&cs, p, strdup("cherry"));
  printf("%-10s = %s\n", o, config_get_str(&cs, o));
  printf("%-10s = %s\n", p, config_get_str(&cs, p));

  struct Address *a1 = calloc(1, sizeof(*a1));
  a1->personal = strdup("Richard Example");
  a1->mailbox  = strdup("rich@example.com");
  struct Address *a2 = calloc(2, sizeof(*a2));
  a2->personal = strdup("Dave Metaphor");
  a2->mailbox  = strdup("dave@metaphor.com");
  struct Address *a3 = calloc(3, sizeof(*a3));
  a3->personal = strdup("Wendy Simile");
  a3->mailbox  = strdup("wendy@simile.com");
  config_set_addr(&cs, q, a1);
  config_set_addr(&cs, r, a2);
  config_set_addr(&cs, r, a3);
  printf("%-10s = %s\n", q, config_get_addr(&cs, q)->personal);
  printf("%-10s = %s\n", r, config_get_addr(&cs, r)->personal);

  struct Regex *r1 = calloc(1, sizeof(*r1));
  r1->pattern = strdup("name.*");
  struct Regex *r2 = calloc(2, sizeof(*r2));
  r2->pattern = strdup(".*name");
  struct Regex *r3 = calloc(3, sizeof(*r3));
  r3->pattern = strdup("na.*me");
  config_set_rx(&cs, s, r1);
  config_set_rx(&cs, t, r2);
  config_set_rx(&cs, t, r3);
  printf("%-10s = %s\n", s, config_get_rx(&cs, s)->pattern);
  printf("%-10s = %s\n", t, config_get_rx(&cs, t)->pattern);

  struct MbCharTable *m1 = calloc(1, sizeof(*m1));
  m1->segmented_str = strdup("x y z");
  m1->orig_str      = strdup("xyz");
  struct MbCharTable *m2 = calloc(2, sizeof(*m2));
  m2->segmented_str = strdup("u v c");
  m2->orig_str  = strdup("abc");
  struct MbCharTable *m3 = calloc(3, sizeof(*m3));
  m3->segmented_str = strdup("p q r");
  m3->orig_str  = strdup("pqr");
  config_set_mbchartbl(&cs, u, m1);
  config_set_mbchartbl(&cs, v, m2);
  config_set_mbchartbl(&cs, v, m3);
  printf("%-10s = %s\n", u, config_get_mbchartbl(&cs, u)->segmented_str);
  printf("%-10s = %s\n", v, config_get_mbchartbl(&cs, v)->segmented_str);

  config_set_free(&cs);
  return 0;
}
