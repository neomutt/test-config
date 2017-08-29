#ifndef _CONFIG_INHERITANCE_H
#define _CONFIG_INHERITANCE_H

#include <stdint.h>

struct Inheritance
{
  struct HashElem *parent;
  const char *name;
  struct Account *ac;
  intptr_t var;
};

#endif /* _CONFIG_INHERITANCE_H */
