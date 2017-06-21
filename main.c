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

  config_set_int(&cs, a, 42);
  config_set_int(&cs, b, 99);
  config_set_int(&cs, c, -66);

  printf("%-10s = %d\n", a, config_get_int(&cs, a));
  printf("%-10s = %d\n", b, config_get_int(&cs, b));
  printf("%-10s = %d\n", c, config_get_int(&cs, c));
  printf("%-10s = %d\n", d, config_get_int(&cs, d));

  config_set_string(&cs, e, "eel");
  config_set_string(&cs, f, "fox");
  config_set_string(&cs, g, "goat");

  printf("%-10s = %s\n", e, config_get_string(&cs, e));
  printf("%-10s = %s\n", f, config_get_string(&cs, f));
  printf("%-10s = %s\n", g, config_get_string(&cs, g));
  printf("%-10s = %s\n", h, config_get_string(&cs, h));

  config_set_free(&cs);
  return 0;
}
