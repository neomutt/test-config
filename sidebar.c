#include <stdbool.h>
#include <stddef.h>
#include "config_set.h"
#include "mutt_options.h"
#include "type/sort.h"

bool   OPT_SIDEBAR;
bool   OPT_SIDEBAR_FOLDER_INDENT;
bool   OPT_SIDEBAR_NEWMAIL_ONLY;
bool   OPT_SIDEBAR_NEXT_NEW_WRAP;
bool   OPT_SIDEBAR_ON_RIGHT;
bool   OPT_SIDEBAR_SHORT_PATH;
char  *SidebarDelimChars;
char  *SidebarDividerChar;
char  *SidebarFormat;
char  *SidebarIndentString;
short  SidebarSortMethod;
short  SidebarWidth;

struct VariableDef SidebarVars[] = {
  { "sidebar_delim_chars",   DT_STR,                  &SidebarDelimChars,         IP "/." },
  { "sidebar_divider_char",  DT_STR,                  &SidebarDividerChar,        0 },
  { "sidebar_folder_indent", DT_BOOL,                 &OPT_SIDEBAR_FOLDER_INDENT, 0 },
  { "sidebar_format",        DT_STR,                  &SidebarFormat,             IP "%B%* %n" },
  { "sidebar_indent_string", DT_STR,                  &SidebarIndentString,       IP " " },
  { "sidebar_new_mail_only", DT_BOOL,                 &OPT_SIDEBAR_NEWMAIL_ONLY,  0 },
  { "sidebar_next_new_wrap", DT_BOOL,                 &OPT_SIDEBAR_NEXT_NEW_WRAP, 0 },
  { "sidebar_on_right",      DT_BOOL,                 &OPT_SIDEBAR_ON_RIGHT,      0 },
  { "sidebar_short_path",    DT_BOOL,                 &OPT_SIDEBAR_SHORT_PATH,    0 },
  { "sidebar_sort_method",   DT_SORT|DT_SORT_SIDEBAR, &SidebarSortMethod,         SORT_ORDER },
  { "sidebar_visible",       DT_BOOL,                 &OPT_SIDEBAR,               0 },
  { "sidebar_width",         DT_NUM,                  &SidebarWidth,              30 },
  { NULL },
};

void init_sidebar(const struct ConfigSet *cs)
{
  cs_register_variables(cs, SidebarVars);
}

