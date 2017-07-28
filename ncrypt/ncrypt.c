#include "config_set.h"
#include "mutt_options.h"
#include "type/sort.h"
#include "type/regex.h"

bool          OPT_ASK_CERT_LABEL;
bool          OPT_CRYPT_AUTO_ENCRYPT;
bool          OPT_CRYPT_AUTO_PGP;
bool          OPT_CRYPT_AUTO_SIGN;
bool          OPT_CRYPT_AUTO_SMIME;
bool          OPT_CRYPT_CONFIRM_HOOK;
bool          OPT_CRYPT_OPPORTUNISTIC_ENCRYPT;
bool          OPT_CRYPT_REPLY_ENCRYPT;
bool          OPT_CRYPT_REPLY_SIGN;
bool          OPT_CRYPT_REPLY_SIGN_ENCRYPTED;
bool          OPT_CRYPT_TIMESTAMP;
bool          OPT_CRYPT_USE_GPGME;
bool          OPT_CRYPT_USE_PKA;
bool          OPT_PGP_AUTO_DEC;
bool          OPT_PGP_AUTO_INLINE;
bool          OPT_PGP_CHECK_EXIT;
bool          OPT_PGP_CHECK_TRUST;
short         OPT_PGP_ENCRYPT_SELF;
bool          OPT_PGP_IGNORE_SUB;
bool          OPT_PGP_LONG_IDS;
short         OPT_PGP_MIME_AUTO;
bool          OPT_PGP_REPLY_INLINE;
bool          OPT_PGP_RETAINABLE_SIG;
bool          OPT_PGP_SELF_ENCRYPT;
bool          OPT_PGP_SHOW_UNUSABLE;
bool          OPT_PGP_STRICT_ENC;
bool          OPT_SDEFAULT_DECRYPT_KEY;
short         OPT_SMIME_ENCRYPT_SELF;
bool          OPT_SMIME_IS_DEFAULT;
bool          OPT_SMIME_SELF_ENCRYPT;
bool          OPT_USE_GPG_AGENT;
short         OPT_VERIFY_SIG;
char         *PgpClearSignCommand;
char         *PgpDecodeCommand;
char         *PgpDecryptCommand;
struct Regex  PgpDecryptionOkay;
char         *PgpEncryptOnlyCommand;
char         *PgpEncryptSignCommand;
char         *PgpEntryFormat;
char         *PgpExportCommand;
char         *PgpGetkeysCommand;
struct Regex  PgpGoodSign;
char         *PgpImportCommand;
char         *PgpListPubringCommand;
char         *PgpListSecringCommand;
char         *PgpSelfEncryptAs;
char         *PgpSignAs;
char         *PgpSignCommand;
short         PgpSortKeys;
short         PgpTimeout;
char         *PgpVerifyCommand;
char         *PgpVerifyKeyCommand;
char         *SmimeCALocation;
char         *SmimeCertificates;
char         *SmimeCryptAlg;
char         *SmimeDecryptCommand;
char         *SmimeDefaultKey;
char         *SmimeDigestAlg;
char         *SmimeEncryptCommand;
char         *SmimeGetCertCommand;
char         *SmimeGetCertEmailCommand;
char         *SmimeGetSignerCertCommand;
char         *SmimeImportCertCommand;
char         *SmimeKeys;
char         *SmimePk7outCommand;
char         *SmimeSelfEncryptAs;
char         *SmimeSignCommand;
short         SmimeTimeout;
char         *SmimeVerifyCommand;
char         *SmimeVerifyOpaqueCommand;

struct VariableDef NcryptVars[] = {
  { "crypt_autoencrypt",             DT_BOOL,              &OPT_CRYPT_AUTO_ENCRYPT,          0 },
  { "crypt_autopgp",                 DT_BOOL,              &OPT_CRYPT_AUTO_PGP,              1 },
  { "crypt_autosign",                DT_BOOL,              &OPT_CRYPT_AUTO_SIGN,             0 },
  { "crypt_autosmime",               DT_BOOL,              &OPT_CRYPT_AUTO_SMIME,            1 },
  { "crypt_confirmhook",             DT_BOOL,              &OPT_CRYPT_CONFIRM_HOOK,          1 },
  { "crypt_opportunistic_encrypt",   DT_BOOL,              &OPT_CRYPT_OPPORTUNISTIC_ENCRYPT, 0 },
  { "crypt_replyencrypt",            DT_BOOL,              &OPT_CRYPT_REPLY_ENCRYPT,         1 },
  { "crypt_replysign",               DT_BOOL,              &OPT_CRYPT_REPLY_SIGN,            0 },
  { "crypt_replysignencrypted",      DT_BOOL,              &OPT_CRYPT_REPLY_SIGN_ENCRYPTED,  0 },
  { "crypt_timestamp",               DT_BOOL,              &OPT_CRYPT_TIMESTAMP,             1 },
  { "crypt_use_gpgme",               DT_BOOL,              &OPT_CRYPT_USE_GPGME,             0 },
  { "crypt_use_pka",                 DT_BOOL,              &OPT_CRYPT_USE_PKA,               0 },
  { "crypt_verify_sig",              DT_QUAD,              &OPT_VERIFY_SIG,                  MUTT_YES },
  { "pgp_autoinline",                DT_BOOL,              &OPT_PGP_AUTO_INLINE,             0 },
  { "pgp_auto_decode",               DT_BOOL,              &OPT_PGP_AUTO_DEC,                0 },
  { "pgp_check_exit",                DT_BOOL,              &OPT_PGP_CHECK_EXIT,              1 },
  { "pgp_clearsign_command",         DT_STR,               &PgpClearSignCommand,             0 },
  { "pgp_decode_command",            DT_STR,               &PgpDecodeCommand,                0 },
  { "pgp_decryption_okay",           DT_RX,                &PgpDecryptionOkay,               0 },
  { "pgp_decrypt_command",           DT_STR,               &PgpDecryptCommand,               0 },
  { "pgp_encrypt_only_command",      DT_STR,               &PgpEncryptOnlyCommand,           0 },
  { "pgp_encrypt_self",              DT_QUAD,              &OPT_PGP_ENCRYPT_SELF,            MUTT_NO },
  { "pgp_encrypt_sign_command",      DT_STR,               &PgpEncryptSignCommand,           0 },
  { "pgp_entry_format",              DT_STR,               &PgpEntryFormat,                  IP "%4n %t%f %4l/0x%k %-4a %2c %u" },
  { "pgp_export_command",            DT_STR,               &PgpExportCommand,                0 },
  { "pgp_getkeys_command",           DT_STR,               &PgpGetkeysCommand,               0 },
  { "pgp_good_sign",                 DT_RX,                &PgpGoodSign,                     0 },
  { "pgp_ignore_subkeys",            DT_BOOL,              &OPT_PGP_IGNORE_SUB,              1 },
  { "pgp_import_command",            DT_STR,               &PgpImportCommand,                0 },
  { "pgp_list_pubring_command",      DT_STR,               &PgpListPubringCommand,           0 },
  { "pgp_list_secring_command",      DT_STR,               &PgpListSecringCommand,           0 },
  { "pgp_long_ids",                  DT_BOOL,              &OPT_PGP_LONG_IDS,                1 },
  { "pgp_mime_auto",                 DT_QUAD,              &OPT_PGP_MIME_AUTO,               MUTT_ASKYES },
  { "pgp_replyinline",               DT_BOOL,              &OPT_PGP_REPLY_INLINE,            0 },
  { "pgp_retainable_sigs",           DT_BOOL,              &OPT_PGP_RETAINABLE_SIG,          0 },
  { "pgp_self_encrypt",              DT_BOOL,              &OPT_PGP_SELF_ENCRYPT,            0 },
  { "pgp_self_encrypt_as",           DT_STR,               &PgpSelfEncryptAs,                0 },
  { "pgp_show_unusable",             DT_BOOL,              &OPT_PGP_SHOW_UNUSABLE,           1 },
  { "pgp_sign_as",                   DT_STR,               &PgpSignAs,                       0 },
  { "pgp_sign_command",              DT_STR,               &PgpSignCommand,                  0 },
  { "pgp_sort_keys",                 DT_SORT|DT_SORT_KEYS, &PgpSortKeys,                     SORT_ADDRESS },
  { "pgp_strict_enc",                DT_BOOL,              &OPT_PGP_STRICT_ENC,              1 },
  { "pgp_timeout",                   DT_NUM,               &PgpTimeout,                      300 },
  { "pgp_use_gpg_agent",             DT_BOOL,              &OPT_USE_GPG_AGENT,               0 },
  { "pgp_verify_command",            DT_STR,               &PgpVerifyCommand,                0 },
  { "pgp_verify_key_command",        DT_STR,               &PgpVerifyKeyCommand,             0 },
  { "smime_ask_cert_label",          DT_BOOL,              &OPT_ASK_CERT_LABEL,              1 },
  { "smime_ca_location",             DT_PATH,              &SmimeCALocation,                 0 },
  { "smime_certificates",            DT_PATH,              &SmimeCertificates,               0 },
  { "smime_decrypt_command",         DT_STR,               &SmimeDecryptCommand,             0 },
  { "smime_decrypt_use_default_key", DT_BOOL,              &OPT_SDEFAULT_DECRYPT_KEY,        1 },
  { "smime_default_key",             DT_STR,               &SmimeDefaultKey,                 0 },
  { "smime_encrypt_command",         DT_STR,               &SmimeEncryptCommand,             0 },
  { "smime_encrypt_self",            DT_QUAD,              &OPT_SMIME_ENCRYPT_SELF,          MUTT_NO },
  { "smime_encrypt_with",            DT_STR,               &SmimeCryptAlg,                   IP "aes256" },
  { "smime_get_cert_command",        DT_STR,               &SmimeGetCertCommand,             0 },
  { "smime_get_cert_email_command",  DT_STR,               &SmimeGetCertEmailCommand,        0 },
  { "smime_get_signer_cert_command", DT_STR,               &SmimeGetSignerCertCommand,       0 },
  { "smime_import_cert_command",     DT_STR,               &SmimeImportCertCommand,          0 },
  { "smime_is_default",              DT_BOOL,              &OPT_SMIME_IS_DEFAULT,            0 },
  { "smime_keys",                    DT_PATH,              &SmimeKeys,                       0 },
  { "smime_pk7out_command",          DT_STR,               &SmimePk7outCommand,              0 },
  { "smime_self_encrypt",            DT_BOOL,              &OPT_SMIME_SELF_ENCRYPT,          0 },
  { "smime_self_encrypt_as",         DT_STR,               &SmimeSelfEncryptAs,              0 },
  { "smime_sign_command",            DT_STR,               &SmimeSignCommand,                0 },
  { "smime_sign_digest_alg",         DT_STR,               &SmimeDigestAlg,                  IP "sha256" },
  { "smime_timeout",                 DT_NUM,               &SmimeTimeout,                    300 },
  { "smime_verify_command",          DT_STR,               &SmimeVerifyCommand,              0 },
  { "smime_verify_opaque_command",   DT_STR,               &SmimeVerifyOpaqueCommand,        0 },
  { NULL },
};

void init_ncrypt(struct ConfigSet *set)
{
  cs_register_variables(set, NcryptVars);
}

