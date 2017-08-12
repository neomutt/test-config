#ifndef _MUTT_ACCOUNT_H
#define _MUTT_ACCOUNT_H

struct ConfigSet;
struct HashElem;

extern const char *AccountVarStr[];

enum AccountVar
{
  V_FORWARD_QUOTE,
  V_PAGER_STOP,
  V_PGP_IGNORE_SUBKEYS,
  V_POP_USER,
  V_POSTPONE_ENCRYPT,
  V_REFLOW_SPACE_QUOTES,
  V_RESUME_DRAFT_FILES,
  V_SMTP_AUTHENTICATORS,
  V_TMPDIR,
  V_TS_STATUS_FORMAT,
  V_MAX,
};

struct Account
{
  char *name;
  struct ConfigSet *cs;
  struct HashElem *vars[V_MAX];
};

struct Account *account_create(const char *name, struct ConfigSet *cs);
void account_free(struct Account **ac);

#endif /* _MUTT_ACCOUNT_H */
