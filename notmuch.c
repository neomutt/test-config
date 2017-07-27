#include "config_set.h"
#include "mutt_options.h"

int   NotmuchDBLimit;
char *NotmuchDefaultUri;
char *NotmuchExcludeTags;
char *NotmuchHiddenTags;
int   NotmuchOpenTimeout;
char *NotmuchQueryType;
int   NotmuchQueryWindowCurrentPosition;
char *NotmuchQueryWindowCurrentSearch;
int   NotmuchQueryWindowDuration;
char *NotmuchQueryWindowTimebase;
char *NotmuchRecordTags;
char *NotmuchUnreadTag;
bool  OPT_NOTMUCH_RECORD;

#define UL (intptr_t)

struct VariableDef NotmuchVars[] = {
  { "nm_db_limit",                      DT_NUM,  &NotmuchDBLimit,                    0 },
  { "nm_default_uri",                   DT_STR,  &NotmuchDefaultUri,                 0 },
  { "nm_exclude_tags",                  DT_STR,  &NotmuchExcludeTags,                0 },
  { "nm_hidden_tags",                   DT_STR,  &NotmuchHiddenTags,                 UL "unread, draft,flagged,passed,replied,attachment,signed,encrypted" },
  { "nm_open_timeout",                  DT_NUM,  &NotmuchOpenTimeout,                5 },
  { "nm_query_type",                    DT_STR,  &NotmuchQueryType,                  UL "messages" },
  { "nm_query_window_current_position", DT_NUM,  &NotmuchQueryWindowCurrentPosition, 0 },
  { "nm_query_window_current_search",   DT_STR,  &NotmuchQueryWindowCurrentSearch,   UL "" },
  { "nm_query_window_duration",         DT_NUM,  &NotmuchQueryWindowDuration,        0 },
  { "nm_query_window_timebase",         DT_STR,  &NotmuchQueryWindowTimebase,        UL "week" },
  { "nm_record",                        DT_BOOL, &OPT_NOTMUCH_RECORD,                0 },
  { "nm_record_tags",                   DT_STR,  &NotmuchRecordTags,                 0 },
  { "nm_unread_tag",                    DT_STR,  &NotmuchUnreadTag,                  UL "unread" },
  { NULL },
};

void init_notmuch(struct ConfigSet *set)
{
  cs_register_variables(set, NotmuchVars);
}
