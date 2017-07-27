#ifndef _MUTT_GLOBAL_H
#define _MUTT_GLOBAL_H

#include "type/address.h"
#include "type/mbyte_table.h"
#include "type/mutt_regex.h"

extern char               *AliasFile;
extern char               *AliasFmt;
extern char               *AssumedCharset;
extern char               *AttachCharset;
extern char               *AttachFormat;
extern struct Regex        AttachKeyword;
extern char               *AttachSep;
extern char               *Attribution;
extern char               *AttributionLocale;
extern short               BrowserSort;
extern short               BuffyCheckStatsInterval;
extern short               BuffyTimeout;
extern char               *Charset;
extern char               *ComposeFormat;
extern char               *ConfigCharset;
extern short               ConnectTimeout;
extern char               *ContentType;
extern char               *DateFmt;
extern char               *DebugFile;
extern short               DebugLevel;
extern char               *DefaultHook;
extern short               DefaultMagic;
extern char               *DisplayFilter;
extern char               *DsnNotify;
extern char               *DsnReturn;
extern char               *Editor;
extern char               *EmptySubject;
extern struct Address     *EnvFrom;
extern char               *EscChar;
extern struct MbCharTable *Flagchars;
extern char               *FolderFormat;
extern char               *ForwardAttrIntro;
extern char               *ForwardAttrTrailer;
extern char               *ForwFmt;
extern char               *Fqdn;
extern struct Address     *From;
extern struct MbCharTable *Fromchars;
extern char               *GroupFormat;
extern char               *HdrFmt;
extern char               *HeaderCache;
extern char               *HeaderCacheBackend;
extern char               *HeaderCachePageSize;
extern char               *HistFile;
extern short               HistSize;
extern char               *Inbox;
extern char               *Inews;
extern char               *Ispell;
extern char               *MailcapPath;
extern char               *Maildir;
extern char               *MarkMacroPrefix;
extern short               MenuContext;
extern char               *MessageCachedir;
extern char               *MhFlagged;
extern char               *MhReplied;
extern char               *MhUnseen;
extern char               *MixEntryFormat;
extern char               *Mixmaster;
extern char               *MsgFmt;
extern short               NetInc;
extern char               *NewMailCmd;
extern char               *Outbox;
extern char               *Pager;
extern short               PagerContext;
extern char               *PagerFmt;
extern short               PagerIndexLines;
extern char               *PipeSep;
extern char               *PopAuthenticators;
extern short               PopCheckTimeout;
extern char               *PopHost;
extern char               *PopPass;
extern char               *PopUser;
extern char               *PostIndentString;
extern char               *Postponed;
extern char               *PostponeEncryptAs;
extern char               *Preconnect;
extern char               *Prefix;
extern char               *PrintCmd;
extern char               *QueryCmd;
extern char               *QueryFormat;
extern short               ReadInc;
extern char               *Realname;
extern short               ReflowWrap;
extern short               SaveHist;
extern short               ScoreThresholdDelete;
extern short               ScoreThresholdFlag;
extern short               ScoreThresholdRead;
extern short               SearchContext;
extern char               *SendCharset;
extern char               *Sendmail;
extern short               SendmailWait;
extern char               *Shell;
extern char               *ShowMultipartAlternative;
extern char               *SidebarDelimChars;
extern char               *SidebarDividerChar;
extern char               *SidebarFormat;
extern char               *SidebarIndentString;
extern short               SidebarSortMethod;
extern short               SidebarWidth;
extern char               *Signature;
extern char               *SimpleSearch;
extern short               SkipQuotedOffset;
extern short               SleepTime;
extern char               *SmtpAuthenticators;
extern char               *SmtpPass;
extern char               *SmtpUrl;
extern short               Sort;
extern short               SortAlias;
extern short               SortAux;
extern char               *SpamSep;
extern char               *Spoolfile;
extern char               *SslCACertFile;
extern char               *SslCertFile;
extern char               *SslCiphers;
extern char               *SslClientCert;
extern short               SslDHPrimeBits;
extern char               *SslEntropyFile;
extern char               *Status;
extern struct MbCharTable *StChars;
extern char               *Tempdir;
extern short               TimeInc;
extern short               Timeout;
extern struct MbCharTable *Tochars;
extern char               *TrashPath;
extern char               *TSIconFormat;
extern char               *TSStatusFormat;
extern char               *Tunnel;
extern char               *VirtFolderFormat;
extern char               *Visual;
extern short               Wrap;
extern short               WrapHeaders;
extern short               WriteInc;
extern char               *XlabelDelim;

#endif /* _MUTT_GLOBAL_H */
