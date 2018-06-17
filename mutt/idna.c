/**
 * @file
 * Handling of international domain names
 *
 * @authors
 * Copyright (C) 2003,2005,2008-2009 Thomas Roessler <roessler@does-not-exist.org>
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
 * @page idna Handling of international domain names
 *
 * Handling of international domain names
 */

#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "charset.h"
#include "idna2.h"
#include "logging.h"
#include "memory.h"
#include "string2.h"
#ifdef HAVE_STRINGPREP_H
#include <stringprep.h>
#elif defined(HAVE_IDN_STRINGPREP_H)
#include <idn/stringprep.h>
#endif
#ifdef HAVE_IDN2_H
#include <idn2.h>
#elif defined(HAVE_IDN_IDN2_H)
#include <idn/idn2.h>
#elif defined(HAVE_IDNA_H)
#include <idna.h>
#elif defined(HAVE_IDN_IDNA_H)
#include <idn/idna.h>
#endif

#ifdef HAVE_LIBIDN
bool IdnDecode;
bool IdnEncode;
#endif

#ifdef HAVE_LIBIDN
/* Work around incompatibilities in the libidn API */
#if (!defined(HAVE_IDNA_TO_ASCII_8Z) && defined(HAVE_IDNA_TO_ASCII_FROM_UTF8))
#define idna_to_ascii_8z(a, b, c)                                              \
  idna_to_ascii_from_utf8(a, b, (c) &1, ((c) &2) ? 1 : 0)
#endif
#if (!defined(HAVE_IDNA_TO_ASCII_LZ) && defined(HAVE_IDNA_TO_ASCII_FROM_LOCALE))
#define idna_to_ascii_lz(a, b, c)                                              \
  idna_to_ascii_from_locale(a, b, (c) &1, ((c) &2) ? 1 : 0)
#endif
#if (!defined(HAVE_IDNA_TO_UNICODE_8Z8Z) && defined(HAVE_IDNA_TO_UNICODE_UTF8_FROM_UTF8))
#define idna_to_unicode_8z8z(a, b, c)                                          \
  idna_to_unicode_utf8_from_utf8(a, b, (c) &1, ((c) &2) ? 1 : 0)
#endif
#endif /* HAVE_LIBIDN */

#ifdef HAVE_LIBIDN
/**
 * mutt_idna_to_ascii_lz - Convert a domain to Punycode
 * @param input  Domain
 * @param output Result
 * @param flags  Flags, e.g. IDNA_ALLOW_UNASSIGNED
 * @retval 0 Success
 * @retval >0 Failure, error code
 *
 * Convert a domain from the current locale to Punycode.
 *
 * @note The caller must free output
 */
int mutt_idna_to_ascii_lz(const char *input, char **output, int flags)
{
  return idna_to_ascii_lz(input, output, flags);
}

/**
 * check_idn - Is domain in Punycode?
 * @param domain Domain to test
 * @retval true At least one part of domain is in Punycode
 */
static bool check_idn(char *domain)
{
  if (!domain)
    return false;

  if (mutt_str_strncasecmp(domain, "xn--", 4) == 0)
    return true;

  while ((domain = strchr(domain, '.')) != NULL)
  {
    if (mutt_str_strncasecmp(++domain, "xn--", 4) == 0)
      return true;
  }

  return false;
}
#endif /* HAVE_LIBIDN */

/**
 * mutt_idna_intl_to_local - Convert an email's domain from Punycode
 * @param user   Username
 * @param domain Domain
 * @param flags  Flags, e.g. #MI_MAY_BE_IRREVERSIBLE
 * @retval ptr  Newly allocated local email address
 * @retval NULL Error in conversion
 *
 * If #IdnDecode is set, then the domain will be converted from Punycode.
 * For example, "xn--ls8h.la" becomes the emoji domain: ":poop:.la"
 * Then the user and domain are changed from 'utf-8' to the encoding in
 * #Charset.
 *
 * If the flag #MI_MAY_BE_IRREVERSIBLE is NOT given, then the results will be
 * checked to make sure that the transformation is "undo-able".
 *
 * @note The caller must free the returned string.
 */
char *mutt_idna_intl_to_local(const char *user, const char *domain, int flags)
{
  char *mailbox = NULL;
  char *reversed_user = NULL, *reversed_domain = NULL;
  char *tmp = NULL;

  char *local_user = mutt_str_strdup(user);
  char *local_domain = mutt_str_strdup(domain);

#ifdef HAVE_LIBIDN
  bool is_idn_encoded = check_idn(local_domain);
  if (is_idn_encoded && IdnDecode)
  {
    if (idna_to_unicode_8z8z(local_domain, &tmp, IDNA_ALLOW_UNASSIGNED) != IDNA_SUCCESS)
      goto cleanup;
    mutt_str_replace(&local_domain, tmp);
    FREE(&tmp);
  }
#endif /* HAVE_LIBIDN */

  /* we don't want charset-hook effects, so we set flags to 0 */
  if (mutt_ch_convert_string(&local_user, "utf-8", Charset, 0) != 0)
    goto cleanup;

  if (mutt_ch_convert_string(&local_domain, "utf-8", Charset, 0) != 0)
    goto cleanup;

  /* make sure that we can convert back and come out with the same
   * user and domain name.  */
  if ((flags & MI_MAY_BE_IRREVERSIBLE) == 0)
  {
    reversed_user = mutt_str_strdup(local_user);

    if (mutt_ch_convert_string(&reversed_user, Charset, "utf-8", 0) != 0)
    {
      mutt_debug(1, "Not reversible. Charset conv to utf-8 failed for user = '%s'.\n",
                 reversed_user);
      goto cleanup;
    }

    if (mutt_str_strcasecmp(user, reversed_user) != 0)
    {
      mutt_debug(1, "#1 Not reversible. orig = '%s', reversed = '%s'.\n", user, reversed_user);
      goto cleanup;
    }

    reversed_domain = mutt_str_strdup(local_domain);

    if (mutt_ch_convert_string(&reversed_domain, Charset, "utf-8", 0) != 0)
    {
      mutt_debug(1, "Not reversible. Charset conv to utf-8 failed for domain = '%s'.\n",
                 reversed_domain);
      goto cleanup;
    }

#ifdef HAVE_LIBIDN
    /* If the original domain was UTF-8, idna encoding here could
     * produce a non-matching domain!  Thus we only want to do the
     * idna_to_ascii_8z() if the original domain was IDNA encoded.
     */
    if (is_idn_encoded && IdnDecode)
    {
      if (idna_to_ascii_8z(reversed_domain, &tmp, IDNA_ALLOW_UNASSIGNED) != IDNA_SUCCESS)
      {
        mutt_debug(1, "Not reversible. idna_to_ascii_8z failed for domain = '%s'.\n",
                   reversed_domain);
        goto cleanup;
      }
      mutt_str_replace(&reversed_domain, tmp);
    }
#endif /* HAVE_LIBIDN */

    if (mutt_str_strcasecmp(domain, reversed_domain) != 0)
    {
      mutt_debug(1, "#2 Not reversible. orig = '%s', reversed = '%s'.\n", domain, reversed_domain);
      goto cleanup;
    }
  }

  mailbox = mutt_mem_malloc(mutt_str_strlen(local_user) + mutt_str_strlen(local_domain) + 2);
  sprintf(mailbox, "%s@%s", NONULL(local_user), NONULL(local_domain));

cleanup:
  FREE(&local_user);
  FREE(&local_domain);
  FREE(&tmp);
  FREE(&reversed_domain);
  FREE(&reversed_user);

  return mailbox;
}

/**
 * mutt_idna_local_to_intl - Convert an email's domain to Punycode
 * @param user   Username
 * @param domain Domain
 * @retval ptr  Newly allocated Punycode email address
 * @retval NULL Error in conversion
 *
 * The user and domain are assumed to be encoded according to #Charset.
 * They are converted to 'utf-8'.  If #IdnEncode is set, then the domain
 * will be converted to Punycode.  For example, the emoji domain:
 * ":poop:.la" becomes "xn--ls8h.la"
 *
 * @note The caller must free the returned string.
 */
char *mutt_idna_local_to_intl(const char *user, const char *domain)
{
  char *mailbox = NULL;
  char *tmp = NULL;

  char *intl_user = mutt_str_strdup(user);
  char *intl_domain = mutt_str_strdup(domain);

  /* we don't want charset-hook effects, so we set flags to 0 */
  if (mutt_ch_convert_string(&intl_user, Charset, "utf-8", 0) != 0)
    goto cleanup;

  if (mutt_ch_convert_string(&intl_domain, Charset, "utf-8", 0) != 0)
    goto cleanup;

#ifdef HAVE_LIBIDN
  if (IdnEncode)
  {
    if (idna_to_ascii_8z(intl_domain, &tmp, IDNA_ALLOW_UNASSIGNED) != IDNA_SUCCESS)
      goto cleanup;
    mutt_str_replace(&intl_domain, tmp);
  }
#endif /* HAVE_LIBIDN */

  mailbox = mutt_mem_malloc(mutt_str_strlen(intl_user) + mutt_str_strlen(intl_domain) + 2);
  sprintf(mailbox, "%s@%s", NONULL(intl_user), NONULL(intl_domain));

cleanup:
  FREE(&intl_user);
  FREE(&intl_domain);
  FREE(&tmp);

  return mailbox;
}

/**
 * mutt_idna_print_version - Create an IDN version string
 * @retval str Version string
 *
 * @note This is a static string and must not be freed.
 */
const char *mutt_idna_print_version(void)
{
  static char vstring[STRING];

#ifdef HAVE_IDN2_H
  snprintf(vstring, sizeof(vstring), "libidn2: %s (compiled with %s)",
           idn2_check_version(NULL), IDN2_VERSION);
#elif defined(HAVE_LIBIDN)
  snprintf(vstring, sizeof(vstring), "libidn: %s (compiled with %s)",
           stringprep_check_version(NULL), STRINGPREP_VERSION);
#endif

  return vstring;
}
