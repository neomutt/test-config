/**
 * @file
 * Hundreds of global variables to back the user variables
 *
 * @authors
 * Copyright (C) 2017 Richard Russon <rich@flatcap.org>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>
#include "dump/data.h"
#include "config/lib.h"
#include "mailbox.h"

unsigned char   C_AbortNoattach;
struct Regex *  C_AbortNoattachRegex;
unsigned char   C_AbortNosubject;
unsigned char   C_AbortUnmodified;
char *          C_AliasFile;
char *          C_AliasFormat;
bool            C_Allow8bit;
bool            C_AllowAnsi;
bool            C_ArrowCursor;
bool            C_AsciiChars;
bool            C_AskFollowUp;
bool            C_AskXCommentTo;
bool            C_Askbcc;
bool            C_Askcc;
char *          C_AssumedCharset;
char *          C_AttachCharset;
char *          C_AttachFormat;
char *          C_AttachSaveDir;
bool            C_AttachSaveWithoutPrompting;
char *          C_AttachSep;
bool            C_AttachSplit;
char *          C_Attribution;
char *          C_AttributionLocale;
bool            C_AutoSubscribe;
bool            C_AutoTag;
bool            C_Autoedit;
bool            C_Beep;
bool            C_BeepNew;
unsigned char   C_Bounce;
bool            C_BounceDelivered;
bool            C_BrailleFriendly;
bool            C_BrowserAbbreviateMailboxes;
unsigned char   C_CatchupNewsgroup;
char *          C_CertificateFile;
bool            C_ChangeFolderNext;
char *          C_Charset;
bool            C_CheckMboxSize;
bool            C_CheckNew;
bool            C_CollapseAll;
bool            C_CollapseFlagged;
bool            C_CollapseUnread;
char *          C_ComposeFormat;
char *          C_ConfigCharset;
bool            C_Confirmappend;
bool            C_Confirmcreate;
short           C_ConnectTimeout;
char *          C_ContentType;
unsigned char   C_Copy;
bool            C_CryptAutoencrypt;
bool            C_CryptAutopgp;
bool            C_CryptAutosign;
bool            C_CryptAutosmime;
struct MbTable *C_CryptChars;
bool            C_CryptConfirmhook;
bool            C_CryptOpportunisticEncrypt;
bool            C_CryptProtectedHeadersRead;
bool            C_CryptProtectedHeadersSave;
char *          C_CryptProtectedHeadersSubject;
bool            C_CryptProtectedHeadersWrite;
bool            C_CryptReplyencrypt;
bool            C_CryptReplysign;
bool            C_CryptReplysignencrypted;
bool            C_CryptTimestamp;
bool            C_CryptUseGpgme;
bool            C_CryptUsePka;
unsigned char   C_CryptVerifySig;
char *          C_DateFormat;
char *          C_DebugFile;
short           C_DebugLevel;
char *          C_DefaultHook;
unsigned char   C_Delete;
bool            C_DeleteUntag;
bool            C_DigestCollapse;
char *          C_DisplayFilter;
char *          C_DsnNotify;
char *          C_DsnReturn;
bool            C_DuplicateThreads;
bool            C_EditHeaders;
char *          C_Editor;
char *          C_EmptySubject;
bool            C_EncodeFrom;
char *          C_EntropyFile;
struct Address *C_EnvelopeFromAddress;
char *          C_Escape;
char *          C_ExternalSearchCommand;
bool            C_FastReply;
unsigned char   C_FccAttach;
bool            C_FccClear;
struct MbTable *C_FlagChars;
bool            C_FlagSafe;
char *          C_Folder;
char *          C_FolderFormat;
bool            C_FollowupTo;
unsigned char   C_FollowupToPoster;
bool            C_ForceName;
unsigned char   C_ForwardAttachments;
char *          C_ForwardAttributionIntro;
char *          C_ForwardAttributionTrailer;
bool            C_ForwardDecode;
bool            C_ForwardDecrypt;
unsigned char   C_ForwardEdit;
char *          C_ForwardFormat;
bool            C_ForwardQuote;
bool            C_ForwardReferences;
struct Address *C_From;
struct MbTable *C_FromChars;
struct Regex *  C_GecosMask;
char *          C_GroupIndexFormat;
bool            C_Hdrs;
bool            C_Header;
char *          C_HeaderCache;
char *          C_HeaderCacheBackend;
bool            C_HeaderCacheCompress;
char *          C_HeaderCachePagesize;
bool            C_HeaderColorPartial;
bool            C_Help;
bool            C_HiddenHost;
char *          C_HiddenTags;
bool            C_HideLimited;
bool            C_HideMissing;
bool            C_HideThreadSubject;
bool            C_HideTopLimited;
bool            C_HideTopMissing;
short           C_History;
char *          C_HistoryFile;
bool            C_HistoryRemoveDups;
bool            C_HonorDisposition;
unsigned char   C_HonorFollowupTo;
char *          C_Hostname;
bool            C_IdnDecode;
bool            C_IdnEncode;
bool            C_IgnoreLinearWhiteSpace;
bool            C_IgnoreListReplyTo;
char *          C_ImapAuthenticators;
bool            C_ImapCheckSubscribed;
bool            C_ImapCondstore;
char *          C_ImapDelimChars;
long            C_ImapFetchChunkSize;
char *          C_ImapHeaders;
bool            C_ImapIdle;
short           C_ImapKeepalive;
bool            C_ImapListSubscribed;
char *          C_ImapLogin;
char *          C_ImapOauthRefreshCommand;
char *          C_ImapPass;
bool            C_ImapPassive;
bool            C_ImapPeek;
short           C_ImapPipelineDepth;
short           C_ImapPollTimeout;
bool            C_ImapQresync;
bool            C_ImapRfc5161;
bool            C_ImapServernoise;
char *          C_ImapUser;
bool            C_ImplicitAutoview;
unsigned char   C_Include;
bool            C_IncludeEncrypted;
bool            C_IncludeOnlyfirst;
char *          C_IndentString;
char *          C_IndexFormat;
char *          C_Inews;
char *          C_Ispell;
bool            C_KeepFlagged;
short           C_MailCheck;
bool            C_MailCheckRecent;
bool            C_MailCheckStats;
short           C_MailCheckStatsInterval;
char *          C_MailcapPath;
bool            C_MailcapSanitize;
bool            C_MaildirCheckCur;
bool            C_MaildirHeaderCacheVerify;
bool            C_MaildirTrash;
char *          C_MarkMacroPrefix;
bool            C_MarkOld;
bool            C_Markers;
struct Regex *  C_Mask;
char *          C_Mbox;
short           C_MboxType;
short           C_MenuContext;
bool            C_MenuMoveOff;
bool            C_MenuScroll;
bool            C_MessageCacheClean;
char *          C_MessageCachedir;
char *          C_MessageFormat;
bool            C_MetaKey;
bool            C_Metoo;
bool            C_MhPurge;
char *          C_MhSeqFlagged;
char *          C_MhSeqReplied;
char *          C_MhSeqUnseen;
unsigned char   C_MimeForward;
bool            C_MimeForwardDecode;
unsigned char   C_MimeForwardRest;
bool            C_MimeSubject;
char *          C_MimeTypeQueryCommand;
bool            C_MimeTypeQueryFirst;
char *          C_MixEntryFormat;
char *          C_Mixmaster;
unsigned char   C_Move;
bool            C_NarrowTree;
short           C_NetInc;
char *          C_NewMailCommand;
char *          C_NewsCacheDir;
char *          C_NewsServer;
char *          C_NewsgroupsCharset;
char *          C_Newsrc;
short           C_NmDbLimit;
char *          C_NmDefaultUri;
char *          C_NmExcludeTags;
char *          C_NmFlaggedTag;
short           C_NmOpenTimeout;
char *          C_NmQueryType;
short           C_NmQueryWindowCurrentPosition;
char *          C_NmQueryWindowCurrentSearch;
short           C_NmQueryWindowDuration;
char *          C_NmQueryWindowTimebase;
bool            C_NmRecord;
char *          C_NmRecordTags;
char *          C_NmRepliedTag;
char *          C_NmUnreadTag;
char *          C_NntpAuthenticators;
short           C_NntpContext;
bool            C_NntpListgroup;
bool            C_NntpLoadDescription;
char *          C_NntpPass;
short           C_NntpPoll;
char *          C_NntpUser;
char *          C_Pager;
short           C_PagerContext;
char *          C_PagerFormat;
short           C_PagerIndexLines;
bool            C_PagerStop;
bool            C_PgpAutoDecode;
bool            C_PgpAutoinline;
bool            C_PgpCheckExit;
bool            C_PgpCheckGpgDecryptStatusFd;
char *          C_PgpClearsignCommand;
char *          C_PgpDecodeCommand;
char *          C_PgpDecryptCommand;
struct Regex *  C_PgpDecryptionOkay;
char *          C_PgpDefaultKey;
char *          C_PgpEncryptOnlyCommand;
unsigned char   C_PgpEncryptSelf;
char *          C_PgpEncryptSignCommand;
char *          C_PgpEntryFormat;
char *          C_PgpExportCommand;
char *          C_PgpGetkeysCommand;
struct Regex *  C_PgpGoodSign;
bool            C_PgpIgnoreSubkeys;
char *          C_PgpImportCommand;
char *          C_PgpListPubringCommand;
char *          C_PgpListSecringCommand;
bool            C_PgpLongIds;
unsigned char   C_PgpMimeAuto;
bool            C_PgpReplyinline;
bool            C_PgpRetainableSigs;
bool            C_PgpSelfEncrypt;
bool            C_PgpShowUnusable;
char *          C_PgpSignAs;
char *          C_PgpSignCommand;
short           C_PgpSortKeys;
bool            C_PgpStrictEnc;
long            C_PgpTimeout;
bool            C_PgpUseGpgAgent;
char *          C_PgpVerifyCommand;
char *          C_PgpVerifyKeyCommand;
bool            C_PipeDecode;
char *          C_PipeSep;
bool            C_PipeSplit;
bool            C_PopAuthTryAll;
char *          C_PopAuthenticators;
short           C_PopCheckinterval;
unsigned char   C_PopDelete;
char *          C_PopHost;
bool            C_PopLast;
char *          C_PopOauthRefreshCommand;
char *          C_PopPass;
unsigned char   C_PopReconnect;
char *          C_PopUser;
char *          C_PostIndentString;
unsigned char   C_PostModerated;
unsigned char   C_Postpone;
bool            C_PostponeEncrypt;
char *          C_PostponeEncryptAs;
char *          C_Postponed;
char *          C_Preconnect;
char *          C_PreferredLanguages;
unsigned char   C_Print;
char *          C_PrintCommand;
bool            C_PrintDecode;
bool            C_PrintSplit;
bool            C_PromptAfter;
char *          C_QueryCommand;
char *          C_QueryFormat;
unsigned char   C_Quit;
struct Regex *  C_QuoteRegex;
short           C_ReadInc;
bool            C_ReadOnly;
char *          C_Realname;
unsigned char   C_Recall;
char *          C_Record;
bool            C_ReflowSpaceQuotes;
bool            C_ReflowText;
short           C_ReflowWrap;
struct Regex *  C_ReplyRegex;
bool            C_ReplySelf;
unsigned char   C_ReplyTo;
bool            C_ReplyWithXorig;
bool            C_Resolve;
bool            C_ResumeDraftFiles;
bool            C_ResumeEditedDraftFiles;
bool            C_ReverseAlias;
bool            C_ReverseName;
bool            C_ReverseRealname;
bool            C_Rfc2047Parameters;
bool            C_SaveAddress;
bool            C_SaveEmpty;
short           C_SaveHistory;
bool            C_SaveName;
bool            C_SaveUnsubscribed;
bool            C_Score;
short           C_ScoreThresholdDelete;
short           C_ScoreThresholdFlag;
short           C_ScoreThresholdRead;
short           C_SearchContext;
char *          C_SendCharset;
char *          C_Sendmail;
short           C_SendmailWait;
char *          C_Shell;
char *          C_ShowMultipartAlternative;
bool            C_ShowNewNews;
bool            C_ShowOnlyUnread;
short           C_SidebarComponentDepth;
char *          C_SidebarDelimChars;
char *          C_SidebarDividerChar;
bool            C_SidebarFolderIndent;
char *          C_SidebarFormat;
char *          C_SidebarIndentString;
bool            C_SidebarNewMailOnly;
bool            C_SidebarNextNewWrap;
bool            C_SidebarNonEmptyMailboxOnly;
bool            C_SidebarOnRight;
bool            C_SidebarShortPath;
short           C_SidebarSortMethod;
bool            C_SidebarVisible;
short           C_SidebarWidth;
bool            C_SigDashes;
bool            C_SigOnTop;
char *          C_Signature;
char *          C_SimpleSearch;
short           C_SkipQuotedOffset;
short           C_SleepTime;
bool            C_SmartWrap;
struct Regex *  C_Smileys;
bool            C_SmimeAskCertLabel;
char *          C_SmimeCaLocation;
char *          C_SmimeCertificates;
char *          C_SmimeDecryptCommand;
bool            C_SmimeDecryptUseDefaultKey;
char *          C_SmimeDefaultKey;
char *          C_SmimeEncryptCommand;
unsigned char   C_SmimeEncryptSelf;
char *          C_SmimeEncryptWith;
char *          C_SmimeGetCertCommand;
char *          C_SmimeGetCertEmailCommand;
char *          C_SmimeGetSignerCertCommand;
char *          C_SmimeImportCertCommand;
bool            C_SmimeIsDefault;
char *          C_SmimeKeys;
char *          C_SmimePk7outCommand;
bool            C_SmimeSelfEncrypt;
char *          C_SmimeSignAs;
char *          C_SmimeSignCommand;
char *          C_SmimeSignDigestAlg;
short           C_SmimeTimeout;
char *          C_SmimeVerifyCommand;
char *          C_SmimeVerifyOpaqueCommand;
char *          C_SmtpAuthenticators;
char *          C_SmtpOauthRefreshCommand;
char *          C_SmtpPass;
char *          C_SmtpUrl;
short           C_Sort;
short           C_SortAlias;
short           C_SortAux;
short           C_SortBrowser;
bool            C_SortRe;
char *          C_SpamSeparator;
char *          C_Spoolfile;
char *          C_SslCaCertificatesFile;
char *          C_SslCiphers;
char *          C_SslClientCert;
bool            C_SslForceTls;
short           C_SslMinDhPrimeBits;
unsigned char   C_SslStarttls;
bool            C_SslUseSslv2;
bool            C_SslUseSslv3;
bool            C_SslUseTlsv11;
bool            C_SslUseTlsv12;
bool            C_SslUseTlsv1;
bool            C_SslUsesystemcerts;
bool            C_SslVerifyDates;
bool            C_SslVerifyHost;
bool            C_SslVerifyPartialChains;
struct MbTable *C_StatusChars;
char *          C_StatusFormat;
bool            C_StatusOnTop;
bool            C_StrictThreads;
bool            C_Suspend;
bool            C_TextFlowed;
bool            C_ThoroughSearch;
bool            C_ThreadReceived;
bool            C_Tilde;
short           C_TimeInc;
short           C_Timeout;
char *          C_Tmpdir;
struct MbTable *C_ToChars;
char *          C_Trash;
bool            C_TsEnabled;
char *          C_TsIconFormat;
char *          C_TsStatusFormat;
char *          C_Tunnel;
bool            C_UncollapseJump;
bool            C_UncollapseNew;
bool            C_Use8bitmime;
bool            C_UseDomain;
bool            C_UseEnvelopeFrom;
bool            C_UseFrom;
bool            C_UseIpv6;
bool            C_UserAgent;
char *          C_VfolderFormat;
bool            C_VirtualSpoolfile;
char *          C_Visual;
bool            C_WaitKey;
bool            C_Weed;
short           C_Wrap;
short           C_WrapHeaders;
bool            C_WrapSearch;
bool            C_WriteBcc;
short           C_WriteInc;
bool            C_XCommentTo;

int charset_validator(const struct ConfigSet *cs, const struct ConfigDef *cdef,
                      intptr_t value, struct Buffer *err)
{
  return CSR_SUCCESS;
}

int hcache_validator(const struct ConfigSet *cs, const struct ConfigDef *cdef,
                     intptr_t value, struct Buffer *err)
{
  return CSR_SUCCESS;
}

int level_validator(const struct ConfigSet *cs, const struct ConfigDef *cdef,
                    intptr_t value, struct Buffer *err)
{
  return CSR_SUCCESS;
}

int multipart_validator(const struct ConfigSet *cs, const struct ConfigDef *cdef,
                        intptr_t value, struct Buffer *err)
{
  return CSR_SUCCESS;
}

int pager_validator(const struct ConfigSet *cs, const struct ConfigDef *cdef,
                    intptr_t value, struct Buffer *err)
{
  return CSR_SUCCESS;
}

int reply_validator(const struct ConfigSet *cs, const struct ConfigDef *cdef,
                    intptr_t value, struct Buffer *err)
{
  return CSR_SUCCESS;
}

// clang-format off
static struct Mapping MagicMap[] = {
  { "mbox",    MUTT_MBOX,    },
  { "MMDF",    MUTT_MMDF,    },
  { "MH",      MUTT_MH,      },
  { "Maildir", MUTT_MAILDIR, },
  { NULL,      0,            },
};
// clang-format on

struct EnumDef MagicDef = {
  "mbox_type",
  4,
  (struct Mapping *) &MagicMap,
};

