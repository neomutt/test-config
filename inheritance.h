#ifndef _MUTT_INHERITANCE_H
#define _MUTT_INHERITANCE_H

struct HashElem;
struct Account;

struct Inheritance
{
  struct HashElem *parent;
  const char *name;
  struct Account *ac;
};

#endif /* _MUTT_INHERITANCE_H */
