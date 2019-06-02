/**
 * @file
 * API for encryption/signing of emails
 *
 * @authors
 * Copyright (C) 2003 Werner Koch <wk@gnupg.org>
 * Copyright (C) 2004 g10code GmbH
 * Copyright (C) 2019 Pietro Cerutti <gahr@gahr.ch>
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

/**
 * @page ncrypt NCRYPT: Encrypt/decrypt/sign/verify emails
 *
 * Encrypt/decrypt/sign/verify emails
 *
 * | File                             | Description                          |
 * | :------------------------------- | :----------------------------------- |
 * | ncrypt/crypt.c                   | @subpage crypt_crypt                 |
 * | ncrypt/cryptglue.c               | @subpage crypt_cryptglue             |
 * | ncrypt/crypt_gpgme.c             | @subpage crypt_crypt_gpgme           |
 * | ncrypt/crypt_mod.c               | @subpage crypt_crypt_mod             |
 * | ncrypt/crypt_mod_pgp_classic.c   | @subpage crypt_crypt_mod_pgp         |
 * | ncrypt/crypt_mod_pgp_gpgme.c     | @subpage crypt_crypt_mod_pgp_gpgme   |
 * | ncrypt/crypt_mod_smime_classic.c | @subpage crypt_crypt_mod_smime       |
 * | ncrypt/crypt_mod_smime_gpgme.c   | @subpage crypt_crypt_mod_smime_gpgme |
 * | ncrypt/gnupgparse.c              | @subpage crypt_gnupg                 |
 * | ncrypt/pgp.c                     | @subpage crypt_pgp                   |
 * | ncrypt/pgpinvoke.c               | @subpage crypt_pgpinvoke             |
 * | ncrypt/pgpkey.c                  | @subpage crypt_pgpkey                |
 * | ncrypt/pgplib.c                  | @subpage crypt_pgplib                |
 * | ncrypt/pgpmicalg.c               | @subpage crypt_pgpmicalg             |
 * | ncrypt/pgppacket.c               | @subpage crypt_pgppacket             |
 * | ncrypt/smime.c                   | @subpage crypt_smime                 |
 */

#ifndef MUTT_NCRYPT_NCRYPT_H
#define MUTT_NCRYPT_NCRYPT_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef uint16_t SecurityFlags;           ///< Flags, e.g. #SEC_ENCRYPT
#define SEC_NO_FLAGS                  0   ///< No flags are set
#define SEC_ENCRYPT             (1 << 0)  ///< Email is encrypted
#define SEC_SIGN                (1 << 1)  ///< Email is signed
#define SEC_GOODSIGN            (1 << 2)  ///< Email has a valid signature
#define SEC_BADSIGN             (1 << 3)  ///< Email has a bad signature
#define SEC_PARTSIGN            (1 << 4)  ///< Not all parts of the email is signed
#define SEC_SIGNOPAQUE          (1 << 5)  ///< Email has an opaque signature (encrypted)
#define SEC_KEYBLOCK            (1 << 6)  ///< Email has a key attached
#define SEC_INLINE              (1 << 7)  ///< Email has an inline signature
#define SEC_OPPENCRYPT          (1 << 8)  ///< Opportunistic encrypt mode
#define APPLICATION_PGP         (1 << 9)  ///< Use PGP to encrypt/sign
#define APPLICATION_SMIME       (1 << 10) ///< Use SMIME to encrypt/sign
#define PGP_TRADITIONAL_CHECKED (1 << 11) ///< Email has a traditional (inline) signature

#define SEC_ALL_FLAGS          ((1 << 12) - 1)

#define PGP_ENCRYPT  (APPLICATION_PGP | SEC_ENCRYPT)
#define PGP_SIGN     (APPLICATION_PGP | SEC_SIGN)
#define PGP_GOODSIGN (APPLICATION_PGP | SEC_GOODSIGN)
#define PGP_KEY      (APPLICATION_PGP | SEC_KEYBLOCK)
#define PGP_INLINE   (APPLICATION_PGP | SEC_INLINE)

#define SMIME_ENCRYPT  (APPLICATION_SMIME | SEC_ENCRYPT)
#define SMIME_SIGN     (APPLICATION_SMIME | SEC_SIGN)
#define SMIME_GOODSIGN (APPLICATION_SMIME | SEC_GOODSIGN)
#define SMIME_BADSIGN  (APPLICATION_SMIME | SEC_BADSIGN)
#define SMIME_OPAQUE   (APPLICATION_SMIME | SEC_SIGNOPAQUE)

#endif /* MUTT_NCRYPT_NCRYPT_H */
