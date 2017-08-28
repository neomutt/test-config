#ifndef _MUTT_INHERITANCE_H
#define _MUTT_INHERITANCE_H

#include <stdint.h>

struct Inheritance
{
  struct HashElem *parent;
  const char *name;
  struct Account *ac;
  intptr_t var;
};

#endif /* _MUTT_INHERITANCE_H */
