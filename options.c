#include <stdbool.h>

// Quad options
short OPT_ABORT;
short OPT_ATTACH;
short OPT_BOUNCE;
short OPT_CATCHUP;
short OPT_COPY;
short OPT_DELETE;
short OPT_FCC_ATTACH;
short OPT_FOLLOW_UP_TO_POSTER;
short OPT_FORW_EDIT;
short OPT_INCLUDE;
short OPT_MF_UP_TO;
short OPT_MIME_FWD;
short OPT_MIME_FWD_REST;
short OPT_MOVE;
short OPT_POP_DELETE;
short OPT_POP_RECONNECT;
short OPT_POSTPONE;
short OPT_PRINT;
short OPT_QUIT;
short OPT_RECALL;
short OPT_REPLY_TO;
short OPT_SSL_START_TLS;
short OPT_SUBJECT;
short OPT_TO_MODERATED;

// Boolean options
bool OPT_ALLOW_8BIT;
bool OPT_ALLOW_ANSI;
bool OPT_ARROW_CURSOR;
bool OPT_ASCII_CHARS;
bool OPT_ASK_BCC;
bool OPT_ASK_CC;
bool OPT_ASK_FOLLOWUP;
bool OPT_ASK_XCOMMENTTO;
bool OPT_ATTACH_MSG;
bool OPT_ATTACH_SPLIT;
bool OPT_AUTO_EDIT;
bool OPT_AUTO_TAG;
bool OPT_AUX_SORT;
bool OPT_BEEP;
bool OPT_BEEP_NEW;
bool OPT_BOUNCE_DELIVERED;
bool OPT_BRAILLE_FRIENDLY;
bool OPT_CHECK_MBOX_SIZE;
bool OPT_CHECK_NEW;
bool OPT_COLLAPSE_ALL;
bool OPT_COLLAPSE_FLAGGED;
bool OPT_COLLAPSE_UNREAD;
bool OPT_CONFIRM_APPEND;
bool OPT_CONFIRM_CREATE;
bool OPT_DELETE_UNTAG;
bool OPT_DIGEST_COLLAPSE;
bool OPT_DONT_HANDLE_PGP_KEYS;
bool OPT_DUP_THREADS;
bool OPT_EDIT_HDRS;
bool OPT_ENCODE_FROM;
bool OPT_ENV_FROM;
bool OPT_FAST_REPLY;
bool OPT_FCC_CLEAR;
bool OPT_FLAG_SAFE;
bool OPT_FOLLOW_UP_TO;
bool OPT_FORCE_NAME;
bool OPT_FORCE_REFRESH;
bool OPT_FORW_DECODE;
bool OPT_FORW_DECRYPT;
bool OPT_FORW_QUOTE;
bool OPT_FORW_REF;
bool OPT_HCACHE_VERIFY;
bool OPT_HDRS;
bool OPT_HEADER;
bool OPT_HEADER_COLOR_PARTIAL;
bool OPT_HELP;
bool OPT_HIDDEN_HOST;
bool OPT_HIDE_LIMITED;
bool OPT_HIDE_MISSING;
bool OPT_HIDE_READ;
bool OPT_HIDE_THREAD_SUBJECT;
bool OPT_HIDE_TOP_LIMITED;
bool OPT_HIDE_TOP_MISSING;
bool OPT_HIST_REMOVE_DUPS;
bool OPT_HONOR_DISP;
bool OPT_IDN_DECODE;
bool OPT_IDN_ENCODE;
bool OPT_IGNORE_LIST_REPLY_TO;
bool OPT_IGNORE_LWS;
bool OPT_IGNORE_MACRO_EVENTS;
bool OPT_IMPLICIT_AUTOVIEW;
bool OPT_INCLUDE_ONLY_FIRST;
bool OPT_KEEP_FLAGGED;
bool OPT_KEEP_QUIET;
bool OPT_KEYWORDS_LEGACY;
bool OPT_KEYWORDS_STANDARD;
bool OPT_LIST_GROUP;
bool OPT_LOAD_DESC;
bool OPT_LOCALES;
bool OPT_MAILCAP_SANITIZE;
bool OPT_MAIL_CHECK_RECENT;
bool OPT_MAIL_CHECK_STATS;
bool OPT_MAILDIR_CHECK_CUR;
bool OPT_MAILDIR_TRASH;
bool OPT_MARKERS;
bool OPT_MARK_OLD;
bool OPT_MENU_CALLER;
bool OPT_MENU_MOVE_OFF;
bool OPT_MENU_SCROLL;
bool OPT_MESSAGE_CACHE_CLEAN;
bool OPT_METAKEY;
bool OPT_ME_TOO;
bool OPT_MH_PURGE;
bool OPT_MIME_FORW_DECODE;
bool OPT_MIME_SUBJECT;
bool OPT_MSG_ERR;
bool OPT_NARROW_TREE;
bool OPT_NEED_RESCORE;
bool OPT_NEED_RESORT;
bool OPT_NEWS;
bool OPT_NEWS_SEND;
bool OPT_NO_CURSES;
bool OPT_PAGER_STOP;
bool OPT_PIPE_DECODE;
bool OPT_PIPE_SPLIT;
bool OPT_POP_AUTH_TRY_ALL;
bool OPT_POP_LAST;
bool OPT_POSTPONE_ENCRYPT;
bool OPT_PRINT_DECODE;
bool OPT_PRINT_SPLIT;
bool OPT_PROMPT_AFTER;
bool OPT_READONLY;
bool OPT_REDRAW_TREE;
bool OPT_REFLOW_SPACE_QUOTES;
bool OPT_REFLOW_TEXT;
bool OPT_REPLY_SELF;
bool OPT_REPLY_WITH_XORIG;
bool OPT_RESOLVE;
bool OPT_RESORT_INIT;
bool OPT_RESUME_DRAFT_FILES;
bool OPT_RESUME_EDITED_DRAFT_FILES;
bool OPT_REV_ALIAS;
bool OPT_REV_NAME;
bool OPT_REV_REAL;
bool OPT_RFC2047_PARAMS;
bool OPT_SAVE_ADDRESS;
bool OPT_SAVE_EMPTY;
bool OPT_SAVE_NAME;
bool OPT_SAVE_UNSUB;
bool OPT_SCORE;
bool OPT_SEARCH_INVALID;
bool OPT_SEARCH_REVERSE;
bool OPT_SHOW_NEW_NEWS;
bool OPT_SHOW_ONLY_UNREAD;
bool OPT_SIDEBAR;
bool OPT_SIDEBAR_FOLDER_INDENT;
bool OPT_SIDEBAR_NEWMAIL_ONLY;
bool OPT_SIDEBAR_NEXT_NEW_WRAP;
bool OPT_SIDEBAR_ON_RIGHT;
bool OPT_SIDEBAR_SHORT_PATH;
bool OPT_SIG_DASHES;
bool OPT_SIGNALS_BLOCKED;
bool OPT_SIG_ON_TOP;
bool OPT_SORT_RE;
bool OPT_SORT_SUBTHREADS;
bool OPT_SSL_FORCE_TLS;
bool OPT_SSL_SYSTEM_CERTS;
bool OPT_SSLV2;
bool OPT_SSLV3;
bool OPT_SSL_VERIFY_DATES;
bool OPT_SSL_VERIFY_HOST;
bool OPT_SSL_VERIFY_PARTIAL;
bool OPT_STATUS_ON_TOP;
bool OPT_STRICT_THREADS;
bool OPT_SUSPEND;
bool OPT_SYS_SIGNALS_BLOCKED;
bool OPT_TEXT_FLOWED;
bool OPT_THOROUGH_SRC;
bool OPT_THREAD_RECEIVED;
bool OPT_TILDE;
bool OPT_TLSV1;
bool OPT_TLSV1_1;
bool OPT_TLSV1_2;
bool OPT_TS_ENABLED;
bool OPT_UNCOLLAPSE_JUMP;
bool OPT_UNCOLLAPSE_NEW;
bool OPT_USE_8BIT_MIME;
bool OPT_USE_DOMAIN;
bool OPT_USE_FROM;
bool OPT_USE_IPV6;
bool OPT_VIEW_ATTACH;
bool OPT_VIRT_SPOOL_FILE;
bool OPT_WAIT_KEY;
bool OPT_WEED;
bool OPT_WRAP;
bool OPT_WRAP_SEARCH;
bool OPT_WRITE_BCC;
bool OPT_XCOMMENT_TO;
bool OPT_XMAILER;
