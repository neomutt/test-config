#ifndef _CONFIG_H
#define _CONFIG_H
#define _GNU_SOURCE 1
#define ALL_TARGETS "all-po all-doc all-contrib all-test"
#define BINDIR "/usr/bin"
#define CLEAN_TARGETS "clean-po clean-doc clean-contrib clean-test"
#define CRYPT_BACKEND_CLASSIC_PGP 1
#define CRYPT_BACKEND_CLASSIC_SMIME 1
#define CRYPT_BACKEND_GPGME 1
#define ENABLE_COVERAGE 1
#define ENABLE_NLS 1
#define ENABLE_UNIT_TESTS 1
#define HAVE_BDB 1
#define HAVE_BIND_TEXTDOMAIN_CODESET 1
#define HAVE_BKGDSET 1
#define HAVE_CFLAG_STD_C99 1
#define HAVE_CLOCK_GETTIME 1
#define HAVE_COLOR 1
#define HAVE_CURS_SET 1
#define HAVE_DB_ENV_CREATE 1
#define HAVE_DB_H 1
#define HAVE_DECL_GNUTLS_VERIFY_DISABLE_TIME_CHECKS 1
#define HAVE_DECL_SYS_SIGLIST 1
#define HAVE_FGETC_UNLOCKED 1
#define HAVE_FUTIMENS 1
#define HAVE_GDBM 1
#define HAVE_GDBM_H 1
#define HAVE_GDBM_OPEN 1
#define HAVE_GETADDRINFO 1
#define HAVE_GETADDRINFO_A 1
#define HAVE_GETHOSTENT 1
#define HAVE_GETSID 1
#define HAVE_GNUTLS_CERTIFICATE_CREDENTIALS_T 1
#define HAVE_GNUTLS_CERTIFICATE_STATUS_T 1
#define HAVE_GNUTLS_CHECK_VERSION 1
#define HAVE_GNUTLS_DATUM_T 1
#define HAVE_GNUTLS_DIGEST_ALGORITHM_T 1
#define HAVE_GNUTLS_GNUTLS_H 1
#define HAVE_GNUTLS_PRIORITY_SET_DIRECT 1
#define HAVE_GNUTLS_SESSION_T 1
#define HAVE_GNUTLS_TRANSPORT_PTR_T 1
#define HAVE_GNUTLS_X509_CRT_T 1
#define HAVE_GNUTLS_X509_H 1
#define HAVE_GPGME 1
#define HAVE_GPGME_H 1
#define HAVE_GPGME_NEW 1
#define HAVE_ICONV 1
#define HAVE_ICONV_H 1
#define HAVE_IDN2_CHECK_VERSION 1
#define HAVE_IDN2_H 1
#define HAVE_IDN2_TO_ASCII_8Z 1
#define HAVE_IDN2_TO_UNICODE_8Z8Z 1
#define HAVE_INOTIFY_ADD_WATCH 1
#define HAVE_INOTIFY_INIT1 1
#define HAVE_INOTIFY_RM_WATCH 1
/* #undef HAVE_IOCTL_H */
#define HAVE_ISWBLANK 1
#define HAVE_KC 1
#define HAVE_KCDBOPEN 1
#define HAVE_KCLANGC_H 1
#define HAVE_LFS 1
#define HAVE_LIBIDN 1
#define HAVE_LITTLE_ENDIAN 1
#define HAVE_LMDB 1
#define HAVE_LMDB_H 1
#define HAVE_LUAL_OPENLIBS 1
#define HAVE_MDB_ENV_CREATE 1
#define HAVE_META 1
/* #undef HAVE_MINIX_CONFIG_H */
#define HAVE_MKDTEMP 1
#define HAVE_NCURSESW_NCURSES_H 1
#define HAVE_NOTMUCH 1
#define HAVE_NOTMUCH_DATABASE_INDEX_FILE 1
#define HAVE_NOTMUCH_DATABASE_OPEN 1
#define HAVE_NOTMUCH_H 1
#define HAVE_PGP 1
#define HAVE_QDBM 1
#define HAVE_QDBM_VILLA_H 1
#define HAVE_SASL 1
#define HAVE_SASL_CLIENT_DONE 1
#define HAVE_SASL_ENCODE64 1
#define HAVE_SASL_SASL_H 1
#define HAVE_SETSOCKOPT 1
#define HAVE_SIGNAL_H 1
#define HAVE_SMIME 1
#define HAVE_START_COLOR 1
#define HAVE_STDLIB_H 1
#define HAVE_STRSEP 1
#define HAVE_STRUCT_STAT_ST_ATIM_TV_NSEC 1
#define HAVE_STRUCT_TIMESPEC 1
#define HAVE_SYSEXITS_H 1
#define HAVE_SYS_INOTIFY_H 1
#define HAVE_SYS_IOCTL_H 1
#define HAVE_SYS_PARAM_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_SYSCALL_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_TC 1
#define HAVE_TCBDBOPEN 1
#define HAVE_TCBDB_H 1
#define HAVE_TGETENT 1
#define HAVE_TIME_H 1
#define HAVE_TYPEAHEAD 1
#define HAVE_UNISTD_H 1
#define HAVE_USE_DEFAULT_COLORS 1
#define HAVE_USE_EXTENDED_NAMES 1
/* #undef HAVE_UTIMESNSAT */
#define HAVE_VASPRINTF 1
#define HAVE_VLOPEN 1
#define HAVE_WADDNWSTR 1
#define HAVE_WCSCASECMP 1
#define ICONV_CONST 
#define INSTALL_TARGETS "install-po install-doc install-contrib install-test"
#define LOFF_T off_t
#define MAILPATH "/var/mail"
#define MIXMASTER "mixmaster"
#define MUTTLOCALEDIR "/usr/share/locale"
#define NOTMUCH_API_3 1
#define OFF_T_FMT "%" PRId64
#define PACKAGE "neomutt"
#define PACKAGE_VERSION "20180716"
#define PKGDATADIR "/usr/share/neomutt"
#define PKGDOCDIR "/usr/share/doc/neomutt"
#define SENDMAIL "/usr/sbin/sendmail"
#define SIG_ATOMIC_VOLATILE_T volatile sig_atomic_t
#define SIZEOF_OFF_T 8
#define SUN_ATTACHMENT 1
#define SYSCONFDIR "/etc"
#define UNINSTALL_TARGETS "uninstall-po uninstall-doc uninstall-contrib uninstall-test"
#define USE_COMPRESSED 1
#define USE_FCNTL 1
/* #undef USE_FMEMOPEN */
#define USE_GSS 1
#define USE_HCACHE 1
#define USE_IMAP 1
#define USE_INOTIFY 1
#define USE_LUA 1
#define USE_NNTP 1
#define USE_NOTMUCH 1
#define USE_POP 1
#define USE_SASL 1
#define USE_SIDEBAR 1
#define USE_SMTP 1
#define USE_SOCKET 1
#define USE_SSL 1
#define USE_SSL_GNUTLS 1
#endif
