#include "config.h"
#include <stdio.h>
#include "config_set.h"

const char *a = "apple";
const char *b = "banana";
const char *c = "cherry";
const char *d = "damson";
const char *e = "elder";
const char *f = "fig";
const char *g = "guava";
const char *h = "haw";

int main()
{
  struct ConfigSet cs;
  config_set_init(&cs);

  config_set_num(&cs, a, 42);
  config_set_num(&cs, b, 99);
  config_set_num(&cs, c, -66);

  printf("%-10s = %d\n", a, config_get_num(&cs, a));
  printf("%-10s = %d\n", b, config_get_num(&cs, b));
  printf("%-10s = %d\n", c, config_get_num(&cs, c));
  printf("%-10s = %d\n", d, config_get_num(&cs, d));

  config_set_str(&cs, e, "eel");
  config_set_str(&cs, f, "fox");
  config_set_str(&cs, g, "goat");

  printf("%-10s = %s\n", e, config_get_str(&cs, e));
  printf("%-10s = %s\n", f, config_get_str(&cs, f));
  printf("%-10s = %s\n", g, config_get_str(&cs, g));
  printf("%-10s = %s\n", h, config_get_str(&cs, h));

  config_set_free(&cs);
  return 0;
}
