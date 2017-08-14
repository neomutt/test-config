#include <stdbool.h>
#include <stddef.h>
#include "config_set.h"
#include "mutt_options.h"

char  *ImapAuthenticators;
char  *ImapDelimChars;
char  *ImapHeaders;
short  ImapKeepalive;
char  *ImapLogin;
char  *ImapPass;
short  ImapPipelineDepth;
char  *ImapUser;
bool   OPT_IMAP_CHECK_SUBSCRIBED;
bool   OPT_IMAP_IDLE;
bool   OPT_IMAP_LSUB;
bool   OPT_IMAP_PASSIVE;
bool   OPT_IMAP_PEEK;
bool   OPT_IMAP_SERVER_NOISE;

const struct VariableDef ImapVars[] = {
  { "imap_authenticators",   DT_STR,  &ImapAuthenticators,        0 },
  { "imap_check_subscribed", DT_BOOL, &OPT_IMAP_CHECK_SUBSCRIBED, 0 },
  { "imap_delim_chars",      DT_STR,  &ImapDelimChars,            IP "/." },
  { "imap_headers",          DT_STR,  &ImapHeaders,               0 },
  { "imap_idle",             DT_BOOL, &OPT_IMAP_IDLE,             0 },
  { "imap_keepalive",        DT_NUM,  &ImapKeepalive,             300 },
  { "imap_list_subscribed",  DT_BOOL, &OPT_IMAP_LSUB,             0 },
  { "imap_login",            DT_STR,  &ImapLogin,                 0 },
  { "imap_pass",             DT_STR,  &ImapPass,                  0 },
  { "imap_passive",          DT_BOOL, &OPT_IMAP_PASSIVE,          1 },
  { "imap_peek",             DT_BOOL, &OPT_IMAP_PEEK,             1 },
  { "imap_pipeline_depth",   DT_NUM,  &ImapPipelineDepth,         15 },
  { "imap_servernoise",      DT_BOOL, &OPT_IMAP_SERVER_NOISE,     1 },
  { "imap_user",             DT_STR,  &ImapUser,                  0 },
  { NULL },
};

void init_imap(struct ConfigSet *cs)
{
  cs_register_variables(cs, ImapVars);
}

