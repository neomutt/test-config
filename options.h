#ifndef _MUTT_OPT_H
#define _MUTT_OPT_H

#include <stdbool.h>

// Quad options
extern short OPT_ABORT;
extern short OPT_ATTACH;
extern short OPT_BOUNCE;
extern short OPT_CATCHUP;
extern short OPT_COPY;
extern short OPT_DELETE;
extern short OPT_FCC_ATTACH;
extern short OPT_FOLLOW_UP_TO_POSTER;
extern short OPT_FORW_EDIT;
extern short OPT_INCLUDE;
extern short OPT_MF_UP_TO;
extern short OPT_MIME_FWD;
extern short OPT_MIME_FWD_REST;
extern short OPT_MOVE;
extern short OPT_PGP_ENCRYPT_SELF;
extern short OPT_PGP_MIME_AUTO;
extern short OPT_POP_DELETE;
extern short OPT_POP_RECONNECT;
extern short OPT_POSTPONE;
extern short OPT_PRINT;
extern short OPT_QUIT;
extern short OPT_RECALL;
extern short OPT_REPLY_TO;
extern short OPT_SMIME_ENCRYPT_SELF;
extern short OPT_SSL_START_TLS;
extern short OPT_SUBJECT;
extern short OPT_TO_MODERATED;

// Boolean options
extern bool OPT_ALLOW_8BIT;
extern bool OPT_ALLOW_ANSI;
extern bool OPT_ARROW_CURSOR;
extern bool OPT_ASCII_CHARS;
extern bool OPT_ASK_BCC;
extern bool OPT_ASK_CC;
extern bool OPT_ASK_FOLLOWUP;
extern bool OPT_ASK_XCOMMENTTO;
extern bool OPT_ATTACH_MSG;
extern bool OPT_ATTACH_SPLIT;
extern bool OPT_AUTO_EDIT;
extern bool OPT_AUTO_TAG;
extern bool OPT_AUX_SORT;
extern bool OPT_BEEP;
extern bool OPT_BEEP_NEW;
extern bool OPT_BOUNCE_DELIVERED;
extern bool OPT_BRAILLE_FRIENDLY;
extern bool OPT_CHECK_MBOX_SIZE;
extern bool OPT_CHECK_NEW;
extern bool OPT_COLLAPSE_ALL;
extern bool OPT_COLLAPSE_FLAGGED;
extern bool OPT_COLLAPSE_UNREAD;
extern bool OPT_CONFIRM_APPEND;
extern bool OPT_CONFIRM_CREATE;
extern bool OPT_CRYPT_AUTO_ENCRYPT;
extern bool OPT_CRYPT_AUTO_PGP;
extern bool OPT_CRYPT_AUTO_SIGN;
extern bool OPT_CRYPT_AUTO_SMIME;
extern bool OPT_CRYPT_CONFIRM_HOOK;
extern bool OPT_CRYPT_OPPORTUNISTIC_ENCRYPT;
extern bool OPT_CRYPT_REPLY_ENCRYPT;
extern bool OPT_CRYPT_REPLY_SIGN;
extern bool OPT_CRYPT_REPLY_SIGN_ENCRYPTED;
extern bool OPT_CRYPT_TIMESTAMP;
extern bool OPT_CRYPT_USE_GPGME;
extern bool OPT_CRYPT_USE_PKA;
extern bool OPT_DELETE_UNTAG;
extern bool OPT_DIGEST_COLLAPSE;
extern bool OPT_DONT_HANDLE_PGP_KEYS;
extern bool OPT_DUP_THREADS;
extern bool OPT_EDIT_HDRS;
extern bool OPT_ENCODE_FROM;
extern bool OPT_ENV_FROM;
extern bool OPT_FAST_REPLY;
extern bool OPT_FCC_CLEAR;
extern bool OPT_FLAG_SAFE;
extern bool OPT_FOLLOW_UP_TO;
extern bool OPT_FORCE_NAME;
extern bool OPT_FORCE_REFRESH;
extern bool OPT_FORW_DECODE;
extern bool OPT_FORW_DECRYPT;
extern bool OPT_FORW_QUOTE;
extern bool OPT_FORW_REF;
extern bool OPT_HCACHE_COMPRESS;
extern bool OPT_HCACHE_VERIFY;
extern bool OPT_HDRS;
extern bool OPT_HEADER;
extern bool OPT_HEADER_COLOR_PARTIAL;
extern bool OPT_HELP;
extern bool OPT_HIDDEN_HOST;
extern bool OPT_HIDE_LIMITED;
extern bool OPT_HIDE_MISSING;
extern bool OPT_HIDE_READ;
extern bool OPT_HIDE_THREAD_SUBJECT;
extern bool OPT_HIDE_TOP_LIMITED;
extern bool OPT_HIDE_TOP_MISSING;
extern bool OPT_HIST_REMOVE_DUPS;
extern bool OPT_HONOR_DISP;
extern bool OPT_IDN_DECODE;
extern bool OPT_IDN_ENCODE;
extern bool OPT_IGNORE_LIST_REPLY_TO;
extern bool OPT_IGNORE_LWS;
extern bool OPT_IGNORE_MACRO_EVENTS;
extern bool OPT_IMPLICIT_AUTOVIEW;
extern bool OPT_INCLUDE_ONLY_FIRST;
extern bool OPT_KEEP_FLAGGED;
extern bool OPT_KEEP_QUIET;
extern bool OPT_KEYWORDS_LEGACY;
extern bool OPT_KEYWORDS_STANDARD;
extern bool OPT_LOCALES;
extern bool OPT_MAILCAP_SANITIZE;
extern bool OPT_MAIL_CHECK_RECENT;
extern bool OPT_MAIL_CHECK_STATS;
extern bool OPT_MAILDIR_CHECK_CUR;
extern bool OPT_MAILDIR_TRASH;
extern bool OPT_MARKERS;
extern bool OPT_MARK_OLD;
extern bool OPT_MENU_CALLER;
extern bool OPT_MENU_MOVE_OFF;
extern bool OPT_MENU_SCROLL;
extern bool OPT_MESSAGE_CACHE_CLEAN;
extern bool OPT_METAKEY;
extern bool OPT_ME_TOO;
extern bool OPT_MH_PURGE;
extern bool OPT_MIME_FORW_DECODE;
extern bool OPT_MIME_SUBJECT;
extern bool OPT_MSG_ERR;
extern bool OPT_NARROW_TREE;
extern bool OPT_NEED_RESCORE;
extern bool OPT_NEED_RESORT;
extern bool OPT_NO_CURSES;
extern bool OPT_PAGER_STOP;
extern bool OPT_PGP_AUTO_DEC;
extern bool OPT_PGP_AUTO_INLINE;
extern bool OPT_PGP_CHECK_EXIT;
extern bool OPT_PGP_CHECK_TRUST;
extern bool OPT_PGP_IGNORE_SUB;
extern bool OPT_PGP_LONG_IDS;
extern bool OPT_PGP_REPLY_INLINE;
extern bool OPT_PGP_RETAINABLE_SIG;
extern bool OPT_PGP_SELF_ENCRYPT;
extern bool OPT_PGP_SHOW_UNUSABLE;
extern bool OPT_PGP_STRICT_ENC;
extern bool OPT_PIPE_DECODE;
extern bool OPT_PIPE_SPLIT;
extern bool OPT_POP_AUTH_TRY_ALL;
extern bool OPT_POP_LAST;
extern bool OPT_POSTPONE_ENCRYPT;
extern bool OPT_PRINT_DECODE;
extern bool OPT_PRINT_SPLIT;
extern bool OPT_PROMPT_AFTER;
extern bool OPT_READONLY;
extern bool OPT_REDRAW_TREE;
extern bool OPT_REFLOW_SPACE_QUOTES;
extern bool OPT_REFLOW_TEXT;
extern bool OPT_REPLY_SELF;
extern bool OPT_REPLY_WITH_XORIG;
extern bool OPT_RESOLVE;
extern bool OPT_RESORT_INIT;
extern bool OPT_RESUME_DRAFT_FILES;
extern bool OPT_RESUME_EDITED_DRAFT_FILES;
extern bool OPT_REV_ALIAS;
extern bool OPT_REV_NAME;
extern bool OPT_REV_REAL;
extern bool OPT_RFC2047_PARAMS;
extern bool OPT_SAVE_ADDRESS;
extern bool OPT_SAVE_EMPTY;
extern bool OPT_SAVE_NAME;
extern bool OPT_SAVE_UNSUB;
extern bool OPT_SCORE;
extern bool OPT_SEARCH_INVALID;
extern bool OPT_SEARCH_REVERSE;
extern bool OPT_SHOW_ONLY_UNREAD;
extern bool OPT_SIDEBAR;
extern bool OPT_SIDEBAR_FOLDER_INDENT;
extern bool OPT_SIDEBAR_NEWMAIL_ONLY;
extern bool OPT_SIDEBAR_NEXT_NEW_WRAP;
extern bool OPT_SIDEBAR_ON_RIGHT;
extern bool OPT_SIDEBAR_SHORT_PATH;
extern bool OPT_SIG_DASHES;
extern bool OPT_SIGNALS_BLOCKED;
extern bool OPT_SIG_ON_TOP;
extern bool OPT_SORT_RE;
extern bool OPT_SORT_SUBTHREADS;
extern bool OPT_SSL_FORCE_TLS;
extern bool OPT_SSL_SYSTEM_CERTS;
extern bool OPT_SSLV2;
extern bool OPT_SSLV3;
extern bool OPT_SSL_VERIFY_DATES;
extern bool OPT_SSL_VERIFY_HOST;
extern bool OPT_SSL_VERIFY_PARTIAL;
extern bool OPT_STATUS_ON_TOP;
extern bool OPT_STRICT_THREADS;
extern bool OPT_SUSPEND;
extern bool OPT_SYS_SIGNALS_BLOCKED;
extern bool OPT_TEXT_FLOWED;
extern bool OPT_THOROUGH_SRC;
extern bool OPT_THREAD_RECEIVED;
extern bool OPT_TILDE;
extern bool OPT_TLSV1;
extern bool OPT_TLSV1_1;
extern bool OPT_TLSV1_2;
extern bool OPT_TS_ENABLED;
extern bool OPT_UNCOLLAPSE_JUMP;
extern bool OPT_UNCOLLAPSE_NEW;
extern bool OPT_USE_8BIT_MIME;
extern bool OPT_USE_DOMAIN;
extern bool OPT_USE_FROM;
extern bool OPT_USE_IPV6;
extern bool OPT_VIEW_ATTACH;
extern bool OPT_VIRT_SPOOL_FILE;
extern bool OPT_WAIT_KEY;
extern bool OPT_WEED;
extern bool OPT_WRAP;
extern bool OPT_WRAP_SEARCH;
extern bool OPT_WRITE_BCC;
extern bool OPT_XCOMMENT_TO;
extern bool OPT_XMAILER;

#endif /* _MUTT_OPT_H */
