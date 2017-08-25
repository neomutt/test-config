#include <stdbool.h>
#include <stddef.h>
#include "config_set.h"
#include "mutt_options.h"

bool   OPT_POP_AUTH_TRY_ALL;
short  OPT_POP_DELETE;
bool   OPT_POP_LAST;
short  OPT_POP_RECONNECT;
char  *PopAuthenticators;
short  PopCheckTimeout;
char  *PopHost;
char  *PopPass;
char  *PopUser;

struct VariableDef PopVars[] = {
  { "pop_authenticators", DT_STR,  &PopAuthenticators,    0           },
  { "pop_auth_try_all",   DT_BOOL, &OPT_POP_AUTH_TRY_ALL, 1           },
  { "pop_checkinterval",  DT_NUM,  &PopCheckTimeout,      60          },
  { "pop_delete",         DT_QUAD, &OPT_POP_DELETE,       MUTT_ASKNO  },
  { "pop_host",           DT_STR,  &PopHost,              IP          "" },
  { "pop_last",           DT_BOOL, &OPT_POP_LAST,         0           },
  { "pop_pass",           DT_STR,  &PopPass,              IP          "" },
  { "pop_reconnect",      DT_QUAD, &OPT_POP_RECONNECT,    MUTT_ASKYES },
  { "pop_user",           DT_STR,  &PopUser,              0           },
  { NULL },
};

void init_pop(const struct ConfigSet *cs)
{
  cs_register_variables(cs, PopVars);
}

