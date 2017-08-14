#include <stdbool.h>
#include <stddef.h>
#include "config_set.h"
#include "mutt_options.h"

char  *NewsCacheDir;
char  *NewsgroupsCharset;
short  NewsPollTimeout;
char  *NewsRc;
char  *NewsServer;
char  *NntpAuthenticators;
short  NntpContext;
char  *NntpPass;
char  *NntpUser;
bool   OPT_LIST_GROUP;
bool   OPT_LOAD_DESC;
bool   OPT_NEWS;
bool   OPT_NEWS_SEND;
bool   OPT_SHOW_NEW_NEWS;

const struct VariableDef NntpVars[] = {
  { "newsgroups_charset",    DT_STR,  &NewsgroupsCharset,  IP "utf-8" },
  { "newsrc",                DT_PATH, &NewsRc,             IP "~/.newsrc" },
  { "news_cache_dir",        DT_PATH, &NewsCacheDir,       IP "~/.mutt" },
  { "news_server",           DT_STR,  &NewsServer,         0 },
  { "nntp_authenticators",   DT_STR,  &NntpAuthenticators, 0 },
  { "nntp_context",          DT_NUM,  &NntpContext,        1000 },
  { "nntp_listgroup",        DT_BOOL, &OPT_LIST_GROUP,     1 },
  { "nntp_load_description", DT_BOOL, &OPT_LOAD_DESC,      1 },
  { "nntp_pass",             DT_STR,  &NntpPass,           IP "" },
  { "nntp_poll",             DT_NUM,  &NewsPollTimeout,    60 },
  { "nntp_user",             DT_STR,  &NntpUser,           IP "" },
  { "show_new_news",         DT_BOOL, &OPT_SHOW_NEW_NEWS,  1 },
  { NULL },
};

void init_nntp(struct ConfigSet *cs)
{
  cs_register_variables(cs, NntpVars);
}

