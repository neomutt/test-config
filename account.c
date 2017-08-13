#include <stddef.h>
#include "account.h"
#include "lib/lib.h"
#include "config_set.h"

const char *AccountVarStr[] = {
  // "forward_quote",      "pager_stop",          "pgp_ignore_subkeys",
  // "pop_user",           "postpone_encrypt",    "reflow_space_quotes",
  "resume_draft_files", //"smtp_authenticators", "tmpdir",
  // "ts_status_format",
};

struct Account *account_create(const char *name, struct ConfigSet *cs)
{
  if (!name || !cs)
    return NULL;

  struct Account *ac = safe_calloc(1, sizeof(*ac));
  bool success = true;
  char acname[64];

  for (int i = 0; i < mutt_array_size(AccountVarStr); i++)
  {
    struct HashElem *parent = cs_get_elem(cs, AccountVarStr[i]);
    if (!parent)
    {
      printf("%s doesn't exist\n", AccountVarStr[i]);
      success = false;
      break;
    }

    snprintf(acname, sizeof(acname), "%s:%s", name, AccountVarStr[i]);
    ac->vars[i] = cs_inherit_variable(cs, parent, acname);
    if (!ac->vars[i])
    {
      printf("failed to create %s\n", acname);
      success = false;
      break;
    }
  }

  if (success)
    return ac;

  account_free(&ac);
  return NULL;
}

void account_free(struct Account **ac)
{
  FREE(ac);
}

