#include <stddef.h>
#include "config.h"
#include "config_set.h"
#include "globals.h"
#include "mutt_options.h"
#include "options.h"
#include "type/regex.h"
#include "type/sort.h"

#define MIXMASTER "mixmaster"

/* supported mailbox formats */
enum
{
  MUTT_MBOX = 1,
  MUTT_MMDF,
  MUTT_MH,
  MUTT_MAILDIR,
  MUTT_NNTP,
  MUTT_IMAP,
  MUTT_NOTMUCH,
  MUTT_POP,
  MUTT_COMPRESSED,
};

struct Regex Mask;
struct Regex QuoteRegexp;
struct Regex ReplyRegexp;
struct Regex Smileys;
struct Regex GecosMask;

struct VariableDef MuttVars[] = {
  { "alias_file",                       DT_PATH,                 &AliasFile,                         IP "~/.muttrc" },
  { "attribution",                      DT_STR,                  &Attribution,                       IP "On %d, %n wrote:" },
  { "from",                             DT_ADDR,                 &From,                              0 },
  { "mbox_type",                        DT_MAGIC,                &DefaultMagic,                      MUTT_MBOX },
  { "pager_context",                    DT_NUM,                  &PagerContext,                      7 },
  { "post_moderated",                   DT_QUAD,                 &OPT_TO_MODERATED,                  MUTT_ASKYES },
  { "quote_regexp",                     DT_RX,                   &QuoteRegexp,                       IP "^([ \t]*[|>:}#])+" },
  { "resume_draft_files",               DT_BOOL,                 &OPT_RESUME_DRAFT_FILES,            0 },
  { "sort",                             DT_SORT,                 &Sort,                              SORT_DATE },
  { "status_chars",                     DT_MBCHARTBL,            &StChars,                           0 },

  // { "abort_noattach",                   DT_QUAD,                 &OPT_ATTACH,                        MUTT_NO },
  // { "abort_nosubject",                  DT_QUAD,                 &OPT_SUBJECT,                       MUTT_ASKYES },
  // { "abort_unmodified",                 DT_QUAD,                 &OPT_ABORT,                         MUTT_YES },
  // { "alias_format",                     DT_STR,                  &AliasFmt,                          IP "%4n %2f %t %-10a %r" },
  // { "allow_8bit",                       DT_BOOL,                 &OPT_ALLOW_8BIT,                    1 },
  // { "allow_ansi",                       DT_BOOL,                 &OPT_ALLOW_ANSI,                    0 },
  // { "arrow_cursor",                     DT_BOOL,                 &OPT_ARROW_CURSOR,                  0 },
  // { "ascii_chars",                      DT_BOOL,                 &OPT_ASCII_CHARS,                   0 },
  // { "askbcc",                           DT_BOOL,                 &OPT_ASK_BCC,                       0 },
  // { "askcc",                            DT_BOOL,                 &OPT_ASK_CC,                        0 },
  // { "ask_follow_up",                    DT_BOOL,                 &OPT_ASK_FOLLOWUP,                  0 },
  // { "ask_x_comment_to",                 DT_BOOL,                 &OPT_ASK_XCOMMENTTO,                0 },
  // { "assumed_charset",                  DT_STR,                  &AssumedCharset,                    0 },
  // { "attach_charset",                   DT_STR,                  &AttachCharset,                     0 },
  // { "attach_format",                    DT_STR,                  &AttachFormat,                      IP "%u%D%I %t%4n %T%.40d%> [%.7m/%.10M, %.6e%?C?, %C?, %s] " },
  // { "attach_keyword",                   DT_RX,                   &AttachKeyword,                     IP "\\<(attach|attached|attachments?)\\>" },
  // { "attach_sep",                       DT_STR,                  &AttachSep,                         IP "\n" },
  // { "attach_split",                     DT_BOOL,                 &OPT_ATTACH_SPLIT,                  1 },
  // { "attribution_locale",               DT_STR,                  &AttributionLocale,                 IP "" },
  // { "autoedit",                         DT_BOOL,                 &OPT_AUTO_EDIT,                     0 },
  // { "auto_tag",                         DT_BOOL,                 &OPT_AUTO_TAG,                      0 },
  // { "beep",                             DT_BOOL,                 &OPT_BEEP,                          1 },
  // { "beep_new",                         DT_BOOL,                 &OPT_BEEP_NEW,                      0 },
  // { "bounce",                           DT_QUAD,                 &OPT_BOUNCE,                        MUTT_ASKYES },
  // { "bounce_delivered",                 DT_BOOL,                 &OPT_BOUNCE_DELIVERED,              1 },
  // { "braille_friendly",                 DT_BOOL,                 &OPT_BRAILLE_FRIENDLY,              0 },
  // { "catchup_newsgroup",                DT_QUAD,                 &OPT_CATCHUP,                       MUTT_ASKYES },
  // { "certificate_file",                 DT_PATH,                 &SslCertFile,                       IP "~/.mutt_certificates" },
  // { "charset",                          DT_STR,                  &Charset,                           0 },
  // { "check_mbox_size",                  DT_BOOL,                 &OPT_CHECK_MBOX_SIZE,               0 },
  // { "check_new",                        DT_BOOL,                 &OPT_CHECK_NEW,                     1 },
  // { "collapse_all",                     DT_BOOL,                 &OPT_COLLAPSE_ALL,                  0 },
  // { "collapse_flagged",                 DT_BOOL,                 &OPT_COLLAPSE_FLAGGED,              1 },
  // { "collapse_unread",                  DT_BOOL,                 &OPT_COLLAPSE_UNREAD,               1 },
  // { "compose_format",                   DT_STR,                  &ComposeFormat,                     IP "-- NeoMutt: Compose [Approx. msg size: %l Atts: %a]%>-" },
  // { "config_charset",                   DT_STR,                  &ConfigCharset,                     0 },
  // { "confirmappend",                    DT_BOOL,                 &OPT_CONFIRM_APPEND,                1 },
  // { "confirmcreate",                    DT_BOOL,                 &OPT_CONFIRM_CREATE,                1 },
  // { "connect_timeout",                  DT_NUM,                  &ConnectTimeout,                    30 },
  // { "content_type",                     DT_STR,                  &ContentType,                       IP "text/plain" },
  // { "copy",                             DT_QUAD,                 &OPT_COPY,                          MUTT_YES },
  // { "date_format",                      DT_STR,                  &DateFmt,                           IP "!%a, %b %d, %Y at %I:%M:%S%p %Z" },
  // { "debug_file",                       DT_PATH,                 &DebugFile,                         IP "~/.muttdebug" },
  // { "debug_level",                      DT_NUM,                  &DebugLevel,                        0 },
  // { "default_hook",                     DT_STR,                  &DefaultHook,                       IP "~f %s !~P | (~P ~C %s)" },
  // { "delete",                           DT_QUAD,                 &OPT_DELETE,                        MUTT_ASKYES },
  // { "delete_untag",                     DT_BOOL,                 &OPT_DELETE_UNTAG,                  1 },
  // { "digest_collapse",                  DT_BOOL,                 &OPT_DIGEST_COLLAPSE,               1 },
  // { "display_filter",                   DT_PATH,                 &DisplayFilter,                     IP "" },
  // { "dsn_notify",                       DT_STR,                  &DsnNotify,                         IP "" },
  // { "dsn_return",                       DT_STR,                  &DsnReturn,                         IP "" },
  // { "duplicate_threads",                DT_BOOL,                 &OPT_DUP_THREADS,                   1 },
  // { "editor",                           DT_PATH,                 &Editor,                            0 },
  // { "edit_headers",                     DT_BOOL,                 &OPT_EDIT_HDRS,                     0 },
  // { "empty_subject",                    DT_STR,                  &EmptySubject,                      IP "Re: your mail" },
  // { "encode_from",                      DT_BOOL,                 &OPT_ENCODE_FROM,                   0 },
  // { "entropy_file",                     DT_PATH,                 &SslEntropyFile,                    0 },
  // { "envelope_from_address",            DT_ADDR,                 &EnvFrom,                           0 },
  // { "escape",                           DT_STR,                  &EscChar,                           IP "~" },
  // { "fast_reply",                       DT_BOOL,                 &OPT_FAST_REPLY,                    0 },
  // { "fcc_attach",                       DT_QUAD,                 &OPT_FCC_ATTACH,                    MUTT_YES },
  // { "fcc_clear",                        DT_BOOL,                 &OPT_FCC_CLEAR,                     0 },
  // { "flag_chars",                       DT_MBCHARTBL,            &Flagchars,                         IP "*!DdrONon- " },
  // { "flag_safe",                        DT_BOOL,                 &OPT_FLAG_SAFE,                     0 },
  // { "folder",                           DT_PATH,                 &Maildir,                           IP "~/Mail" },
  // { "folder_format",                    DT_STR,                  &FolderFormat,                      IP "%2C %t %N %F %2l %-8.8u %-8.8g %8s %d %f" },
  // { "followup_to",                      DT_BOOL,                 &OPT_FOLLOW_UP_TO,                  1 },
  // { "followup_to_poster",               DT_QUAD,                 &OPT_FOLLOW_UP_TO_POSTER,           MUTT_ASKYES },
  // { "force_name",                       DT_BOOL,                 &OPT_FORCE_NAME,                    0 },
  // { "forward_attribution_intro",        DT_STR,                  &ForwardAttrIntro,                  IP "----- Forwarded message from %f -----" },
  // { "forward_attribution_trailer",      DT_STR,                  &ForwardAttrTrailer,                IP "----- End forwarded message -----" },
  // { "forward_decode",                   DT_BOOL,                 &OPT_FORW_DECODE,                   1 },
  // { "forward_decrypt",                  DT_BOOL,                 &OPT_FORW_DECRYPT,                  1 },
  // { "forward_edit",                     DT_QUAD,                 &OPT_FORW_EDIT,                     MUTT_YES },
  // { "forward_format",                   DT_STR,                  &ForwFmt,                           IP "[%a: %s]" },
  // { "forward_quote",                    DT_BOOL,                 &OPT_FORW_QUOTE,                    0 },
  // { "forward_references",               DT_BOOL,                 &OPT_FORW_REF,                      0 },
  // { "from_chars",                       DT_MBCHARTBL,            &Fromchars,                         0 },
  // { "gecos_mask",                       DT_RX,                   &GecosMask,                         IP "^[^,]*" },
  // { "group_index_format",               DT_STR,                  &GroupFormat,                       IP "%4C %M%N %5s %-45.45f %d" },
  // { "hdrs",                             DT_BOOL,                 &OPT_HDRS,                          1 },
  // { "header",                           DT_BOOL,                 &OPT_HEADER,                        0 },
  // { "header_color_partial",             DT_BOOL,                 &OPT_HEADER_COLOR_PARTIAL,          0 },
  // { "help",                             DT_BOOL,                 &OPT_HELP,                          1 },
  // { "hidden_host",                      DT_BOOL,                 &OPT_HIDDEN_HOST,                   0 },
  // { "hide_limited",                     DT_BOOL,                 &OPT_HIDE_LIMITED,                  0 },
  // { "hide_missing",                     DT_BOOL,                 &OPT_HIDE_MISSING,                  1 },
  // { "hide_thread_subject",              DT_BOOL,                 &OPT_HIDE_THREAD_SUBJECT,           1 },
  // { "hide_top_limited",                 DT_BOOL,                 &OPT_HIDE_TOP_LIMITED,              0 },
  // { "hide_top_missing",                 DT_BOOL,                 &OPT_HIDE_TOP_MISSING,              1 },
  // { "history",                          DT_NUM,                  &HistSize,                          10 },
  // { "history_file",                     DT_PATH,                 &HistFile,                          IP "~/.mutthistory" },
  // { "history_remove_dups",              DT_BOOL,                 &OPT_HIST_REMOVE_DUPS,              0 },
  // { "honor_disposition",                DT_BOOL,                 &OPT_HONOR_DISP,                    0 },
  // { "honor_followup_to",                DT_QUAD,                 &OPT_MF_UP_TO,                      MUTT_YES },
  // { "hostname",                         DT_STR,                  &Fqdn,                              0 },
  // { "idn_decode",                       DT_BOOL,                 &OPT_IDN_DECODE,                    1 },
  // { "idn_encode",                       DT_BOOL,                 &OPT_IDN_ENCODE,                    1 },
  // { "ignore_linear_white_space",        DT_BOOL,                 &OPT_IGNORE_LWS,                    0 },
  // { "ignore_list_reply_to",             DT_BOOL,                 &OPT_IGNORE_LIST_REPLY_TO,          0 },
  // { "implicit_autoview",                DT_BOOL,                 &OPT_IMPLICIT_AUTOVIEW,             0 },
  // { "include",                          DT_QUAD,                 &OPT_INCLUDE,                       MUTT_ASKYES },
  // { "include_onlyfirst",                DT_BOOL,                 &OPT_INCLUDE_ONLY_FIRST,            0 },
  // { "indent_string",                    DT_STR,                  &Prefix,                            IP "> " },
  // { "index_format",                     DT_STR,                  &HdrFmt,                            IP "%4C %Z %{%b %d} %-15.15L (%?l?%4l&%4c?) %s" },
  // { "inews",                            DT_PATH,                 &Inews,                             IP "" },
  // { "ispell",                           DT_PATH,                 &Ispell,                            IP ISPELL },
  // { "keep_flagged",                     DT_BOOL,                 &OPT_KEEP_FLAGGED,                  0 },
  // { "keywords_legacy",                  DT_BOOL,                 &OPT_KEYWORDS_LEGACY,               1 },
  // { "keywords_standard",                DT_BOOL,                 &OPT_KEYWORDS_STANDARD,             0 },
  // { "mailcap_path",                     DT_STR,                  &MailcapPath,                       0 },
  // { "mailcap_sanitize",                 DT_BOOL,                 &OPT_MAILCAP_SANITIZE,              1 },
  // { "maildir_check_cur",                DT_BOOL,                 &OPT_MAILDIR_CHECK_CUR,             0 },
  // { "maildir_header_cache_verify",      DT_BOOL,                 &OPT_HCACHE_VERIFY,                 1 },
  // { "maildir_trash",                    DT_BOOL,                 &OPT_MAILDIR_TRASH,                 0 },
  // { "mail_check",                       DT_NUM,                  &BuffyTimeout,                      5 },
  // { "mail_check_recent",                DT_BOOL,                 &OPT_MAIL_CHECK_RECENT,             1 },
  // { "mail_check_stats",                 DT_BOOL,                 &OPT_MAIL_CHECK_STATS,              0 },
  // { "mail_check_stats_interval",        DT_NUM,                  &BuffyCheckStatsInterval,           60 },
  // { "markers",                          DT_BOOL,                 &OPT_MARKERS,                       1 },
  // { "mark_macro_prefix",                DT_STR,                  &MarkMacroPrefix,                   IP "'" },
  // { "mark_old",                         DT_BOOL,                 &OPT_MARK_OLD,                      1 },
  // { "mask",                             DT_RX,                   &Mask,                              IP "!^\\.[^.]" },
  // { "mbox",                             DT_PATH,                 &Inbox,                             IP "~/mbox" },
  // { "menu_context",                     DT_NUM,                  &MenuContext,                       0 },
  // { "menu_move_off",                    DT_BOOL,                 &OPT_MENU_MOVE_OFF,                 1 },
  // { "menu_scroll",                      DT_BOOL,                 &OPT_MENU_SCROLL,                   0 },
  // { "message_cachedir",                 DT_PATH,                 &MessageCachedir,                   0 },
  // { "message_cache_clean",              DT_BOOL,                 &OPT_MESSAGE_CACHE_CLEAN,           0 },
  // { "message_format",                   DT_STR,                  &MsgFmt,                            IP "%s" },
  // { "meta_key",                         DT_BOOL,                 &OPT_METAKEY,                       0 },
  // { "metoo",                            DT_BOOL,                 &OPT_ME_TOO,                        0 },
  // { "mh_purge",                         DT_BOOL,                 &OPT_MH_PURGE,                      0 },
  // { "mh_seq_flagged",                   DT_STR,                  &MhFlagged,                         IP "flagged" },
  // { "mh_seq_replied",                   DT_STR,                  &MhReplied,                         IP "replied" },
  // { "mh_seq_unseen",                    DT_STR,                  &MhUnseen,                          IP "unseen" },
  // { "mime_forward",                     DT_QUAD,                 &OPT_MIME_FWD,                      MUTT_NO },
  // { "mime_forward_decode",              DT_BOOL,                 &OPT_MIME_FORW_DECODE,              0 },
  // { "mime_forward_rest",                DT_QUAD,                 &OPT_MIME_FWD_REST,                 MUTT_YES },
  // { "mime_subject",                     DT_BOOL,                 &OPT_MIME_SUBJECT,                  1 },
  // { "mixmaster",                        DT_PATH,                 &Mixmaster,                         IP MIXMASTER },
  // { "mix_entry_format",                 DT_STR,                  &MixEntryFormat,                    IP "%4n %c %-16s %a" },
  // { "move",                             DT_QUAD,                 &OPT_MOVE,                          MUTT_NO },
  // { "narrow_tree",                      DT_BOOL,                 &OPT_NARROW_TREE,                   0 },
  // { "net_inc",                          DT_NUM,                  &NetInc,                            10 },
  // { "new_mail_command",                 DT_PATH,                 &NewMailCmd,                        IP NULL },
  // { "pager",                            DT_PATH,                 &Pager,                             IP "builtin" },
  // { "pager_format",                     DT_STR,                  &PagerFmt,                          IP "-%Z- %C/%m: %-20.20n %s%* -- (%P)" },
  // { "pager_index_lines",                DT_NUM,                  &PagerIndexLines,                   0 },
  // { "pager_stop",                       DT_BOOL,                 &OPT_PAGER_STOP,                    0 },
  // { "pipe_decode",                      DT_BOOL,                 &OPT_PIPE_DECODE,                   0 },
  // { "pipe_sep",                         DT_STR,                  &PipeSep,                           IP "\\n" },
  // { "pipe_split",                       DT_BOOL,                 &OPT_PIPE_SPLIT,                    0 },
  // { "postpone",                         DT_QUAD,                 &OPT_POSTPONE,                      MUTT_ASKYES },
  // { "postponed",                        DT_PATH,                 &Postponed,                         IP "~/postponed" },
  // { "postpone_encrypt",                 DT_BOOL,                 &OPT_POSTPONE_ENCRYPT,              0 },
  // { "postpone_encrypt_as",              DT_STR,                  &PostponeEncryptAs,                 0 },
  // { "post_indent_string",               DT_STR,                  &PostIndentString,                  IP "" },
  // { "preconnect",                       DT_STR,                  &Preconnect,                        0 },
  // { "print",                            DT_QUAD,                 &OPT_PRINT,                         MUTT_ASKNO },
  // { "print_command",                    DT_PATH,                 &PrintCmd,                          IP "lpr" },
  // { "print_decode",                     DT_BOOL,                 &OPT_PRINT_DECODE,                  1 },
  // { "print_split",                      DT_BOOL,                 &OPT_PRINT_SPLIT,                   0 },
  // { "prompt_after",                     DT_BOOL,                 &OPT_PROMPT_AFTER,                  1 },
  // { "query_command",                    DT_PATH,                 &QueryCmd,                          IP "" },
  // { "query_format",                     DT_STR,                  &QueryFormat,                       IP "%4c %t %-25.25a %-25.25n %?e?(%e)?" },
  // { "quit",                             DT_QUAD,                 &OPT_QUIT,                          MUTT_YES },
  // { "read_inc",                         DT_NUM,                  &ReadInc,                           10 },
  // { "read_only",                        DT_BOOL,                 &OPT_READONLY,                      0 },
  // { "realname",                         DT_STR,                  &Realname,                          0 },
  // { "recall",                           DT_QUAD,                 &OPT_RECALL,                        MUTT_ASKYES },
  // { "record",                           DT_PATH,                 &Outbox,                            IP "~/sent" },
  // { "reflow_space_quotes",              DT_BOOL,                 &OPT_REFLOW_SPACE_QUOTES,           1 },
  // { "reflow_text",                      DT_BOOL,                 &OPT_REFLOW_TEXT,                   1 },
  // { "reflow_wrap",                      DT_NUM,                  &ReflowWrap,                        78 },
  // { "reply_regexp",                     DT_RX,                   &ReplyRegexp,                       IP "^(re([\\[0-9\\]+])*|aw):[ \t]*" },
  // { "reply_self",                       DT_BOOL,                 &OPT_REPLY_SELF,                    0 },
  // { "reply_to",                         DT_QUAD,                 &OPT_REPLY_TO,                      MUTT_ASKYES },
  // { "reply_with_xorig",                 DT_BOOL,                 &OPT_REPLY_WITH_XORIG,              0 },
  // { "resolve",                          DT_BOOL,                 &OPT_RESOLVE,                       1 },
  // { "resume_edited_draft_files",        DT_BOOL,                 &OPT_RESUME_EDITED_DRAFT_FILES,     1 },
  // { "reverse_alias",                    DT_BOOL,                 &OPT_REV_ALIAS,                     0 },
  // { "reverse_name",                     DT_BOOL,                 &OPT_REV_NAME,                      0 },
  // { "reverse_realname",                 DT_BOOL,                 &OPT_REV_REAL,                      1 },
  // { "rfc2047_parameters",               DT_BOOL,                 &OPT_RFC2047_PARAMS,                0 },
  // { "save_address",                     DT_BOOL,                 &OPT_SAVE_ADDRESS,                  0 },
  // { "save_empty",                       DT_BOOL,                 &OPT_SAVE_EMPTY,                    1 },
  // { "save_history",                     DT_NUM,                  &SaveHist,                          0 },
  // { "save_name",                        DT_BOOL,                 &OPT_SAVE_NAME,                     0 },
  // { "save_unsubscribed",                DT_BOOL,                 &OPT_SAVE_UNSUB,                    0 },
  // { "score",                            DT_BOOL,                 &OPT_SCORE,                         1 },
  // { "score_threshold_delete",           DT_NUM,                  &ScoreThresholdDelete,              IP -1 },
  // { "score_threshold_flag",             DT_NUM,                  &ScoreThresholdFlag,                9999 },
  // { "score_threshold_read",             DT_NUM,                  &ScoreThresholdRead,                IP -1 },
  // { "search_context",                   DT_NUM,                  &SearchContext,                     0 },
  // { "sendmail",                         DT_PATH,                 &Sendmail,                          IP SENDMAIL " -oem -oi" },
  // { "sendmail_wait",                    DT_NUM,                  &SendmailWait,                      0 },
  // { "send_charset",                     DT_STR,                  &SendCharset,                       IP "us-ascii:iso-8859-1:utf-8" },
  // { "shell",                            DT_PATH,                 &Shell,                             0 },
  // { "show_multipart_alternative",       DT_STR,                  &ShowMultipartAlternative,          0 },
  // { "show_only_unread",                 DT_BOOL,                 &OPT_SHOW_ONLY_UNREAD,              0 },
  // { "signature",                        DT_PATH,                 &Signature,                         IP "~/.signature" },
  // { "sig_dashes",                       DT_BOOL,                 &OPT_SIG_DASHES,                    1 },
  // { "sig_on_top",                       DT_BOOL,                 &OPT_SIG_ON_TOP,                    0 },
  // { "simple_search",                    DT_STR,                  &SimpleSearch,                      IP "~f %s | ~s %s" },
  // { "skip_quoted_offset",               DT_NUM,                  &SkipQuotedOffset,                  0 },
  // { "sleep_time",                       DT_NUM,                  &SleepTime,                         1 },
  // { "smart_wrap",                       DT_BOOL,                 &OPT_WRAP,                          1 },
  // { "smileys",                          DT_RX,                   &Smileys,                           IP "(>From )|(:[-^]?[][)(><}{|/DP])" },
  // { "smtp_authenticators",              DT_STR,                  &SmtpAuthenticators,                0 },
  // { "smtp_pass",                        DT_STR,                  &SmtpPass,                          0 },
  // { "smtp_url",                         DT_STR,                  &SmtpUrl,                           0 },
  // { "sort_alias",                       DT_SORT|DT_SORT_ALIAS,   &SortAlias,                         SORT_ALIAS },
  // { "sort_aux",                         DT_SORT|DT_SORT_AUX,     &SortAux,                           SORT_DATE },
  // { "sort_browser",                     DT_SORT|DT_SORT_BROWSER, &BrowserSort,                       SORT_ALPHA },
  // { "sort_re",                          DT_BOOL,                 &OPT_SORT_RE,                       1 },
  // { "spam_separator",                   DT_STR,                  &SpamSep,                           IP "," },
  // { "spoolfile",                        DT_PATH,                 &Spoolfile,                         0 },
  // { "ssl_ca_certificates_file",         DT_PATH,                 &SslCACertFile,                     0 },
  // { "ssl_ciphers",                      DT_STR,                  &SslCiphers,                        0 },
  // { "ssl_client_cert",                  DT_PATH,                 &SslClientCert,                     0 },
  // { "ssl_force_tls",                    DT_BOOL,                 &OPT_SSL_FORCE_TLS,                 0 },
  // { "ssl_min_dh_prime_bits",            DT_NUM,                  &SslDHPrimeBits,                    0 },
  // { "ssl_starttls",                     DT_QUAD,                 &OPT_SSL_START_TLS,                 MUTT_YES },
  // { "ssl_usesystemcerts",               DT_BOOL,                 &OPT_SSL_SYSTEM_CERTS,              1 },
  // { "ssl_use_sslv2",                    DT_BOOL,                 &OPT_SSLV2,                         0 },
  // { "ssl_use_sslv3",                    DT_BOOL,                 &OPT_SSLV3,                         0 },
  // { "ssl_use_tlsv1",                    DT_BOOL,                 &OPT_TLSV1,                         1 },
  // { "ssl_use_tlsv1_1",                  DT_BOOL,                 &OPT_TLSV1_1,                       1 },
  // { "ssl_use_tlsv1_2",                  DT_BOOL,                 &OPT_TLSV1_2,                       1 },
  // { "ssl_verify_dates",                 DT_BOOL,                 &OPT_SSL_VERIFY_DATES,              1 },
  // { "ssl_verify_host",                  DT_BOOL,                 &OPT_SSL_VERIFY_HOST,               1 },
  // { "ssl_verify_partial_chains",        DT_BOOL,                 &OPT_SSL_VERIFY_PARTIAL,            0 },
  // { "status_format",                    DT_STR,                  &Status,                            IP "-%r-NeoMutt: %f [Msgs:%?M?%M/?%m%?n? New:%n?%?o? Old:%o?%?d? Del:%d?%?F? Flag:%F?%?t? Tag:%t?%?p? Post:%p?%?b? Inc:%b?%?l? %l?]---(%s/%S)-%>-(%P)---" },
  // { "status_on_top",                    DT_BOOL,                 &OPT_STATUS_ON_TOP,                 0 },
  // { "strict_threads",                   DT_BOOL,                 &OPT_STRICT_THREADS,                0 },
  // { "suspend",                          DT_BOOL,                 &OPT_SUSPEND,                       1 },
  // { "text_flowed",                      DT_BOOL,                 &OPT_TEXT_FLOWED,                   0 },
  // { "thorough_search",                  DT_BOOL,                 &OPT_THOROUGH_SRC,                  1 },
  // { "thread_received",                  DT_BOOL,                 &OPT_THREAD_RECEIVED,               0 },
  // { "tilde",                            DT_BOOL,                 &OPT_TILDE,                         0 },
  // { "timeout",                          DT_NUM,                  &Timeout,                           600 },
  // { "time_inc",                         DT_NUM,                  &TimeInc,                           0 },
  // { "tmpdir",                           DT_PATH,                 &Tempdir,                           0 },
  // { "to_chars",                         DT_MBCHARTBL,            &Tochars,                           IP " +TCFL" },
  // { "trash",                            DT_PATH,                 &TrashPath,                         0 },
  // { "ts_enabled",                       DT_BOOL,                 &OPT_TS_ENABLED,                    0 },
  // { "ts_icon_format",                   DT_STR,                  &TSIconFormat,                      IP "M%?n?AIL&ail?" },
  // { "ts_status_format",                 DT_STR,                  &TSStatusFormat,                    IP "NeoMutt with %?m?%m messages&no messages?%?n? [%n NEW]?" },
  // { "tunnel",                           DT_STR,                  &Tunnel,                            0 },
  // { "uncollapse_jump",                  DT_BOOL,                 &OPT_UNCOLLAPSE_JUMP,               0 },
  // { "uncollapse_new",                   DT_BOOL,                 &OPT_UNCOLLAPSE_NEW,                1 },
  // { "user_agent",                       DT_BOOL,                 &OPT_XMAILER,                       1 },
  // { "use_8bitmime",                     DT_BOOL,                 &OPT_USE_8BIT_MIME,                 0 },
  // { "use_domain",                       DT_BOOL,                 &OPT_USE_DOMAIN,                    1 },
  // { "use_envelope_from",                DT_BOOL,                 &OPT_ENV_FROM,                      0 },
  // { "use_from",                         DT_BOOL,                 &OPT_USE_FROM,                      1 },
  // { "use_ipv6",                         DT_BOOL,                 &OPT_USE_IPV6,                      1 },
  // { "vfolder_format",                   DT_STR,                  &VirtFolderFormat,                  IP "%2C %?n?%4n/& ?%4m %f" },
  // { "virtual_spoolfile",                DT_BOOL,                 &OPT_VIRT_SPOOL_FILE,               0 },
  // { "visual",                           DT_PATH,                 &Visual,                            0 },
  // { "wait_key",                         DT_BOOL,                 &OPT_WAIT_KEY,                      1 },
  // { "weed",                             DT_BOOL,                 &OPT_WEED,                          1 },
  // { "wrap",                             DT_NUM,                  &Wrap,                              0 },
  // { "wrapmargin",                       DT_NUM,                  &Wrap,                              0 },
  // { "wrap_headers",                     DT_NUM,                  &WrapHeaders,                       78 },
  // { "wrap_search",                      DT_BOOL,                 &OPT_WRAP_SEARCH,                   1 },
  // { "write_bcc",                        DT_BOOL,                 &OPT_WRITE_BCC,                     1 },
  // { "write_inc",                        DT_NUM,                  &WriteInc,                          10 },
  // { "xlabel_delimiter",                 DT_STR,                  &XlabelDelim,                       IP "" },
  // { "x_comment_to",                     DT_BOOL,                 &OPT_XCOMMENT_TO,                   0 },
  { NULL },
};

