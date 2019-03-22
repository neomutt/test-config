/**
 * @file
 * Miscellaneous email parsing routines
 *
 * @authors
 * Copyright (C) 2018 Richard Russon <rich@flatcap.org>
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
 * @page email_parse Miscellaneous email parsing routines
 *
 * Miscellaneous email parsing routines
 */

#include "config.h"
#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mutt/mutt.h"
#include "mutt.h"
#include "parse.h"
#include "address.h"
#include "body.h"
#include "email.h"
#include "email_globals.h"
#include "envelope.h"
#include "from.h"
#include "globals.h"
#include "mime.h"
#include "parameter.h"
#include "protos.h"
#include "rfc2047.h"
#include "rfc2231.h"
#include "url.h"

/**
 * mutt_auto_subscribe - Check if user is subscribed to mailing list
 * @param mailto URI of mailing list subscribe
 */
void mutt_auto_subscribe(const char *mailto)
{
  struct Envelope *lpenv;

  if (!AutoSubscribeCache)
    AutoSubscribeCache = mutt_hash_new(200, MUTT_HASH_STRCASECMP | MUTT_HASH_STRDUP_KEYS);

  if (!mailto || mutt_hash_find(AutoSubscribeCache, mailto))
    return;

  mutt_hash_insert(AutoSubscribeCache, mailto, AutoSubscribeCache);

  lpenv = mutt_env_new(); /* parsed envelope from the List-Post mailto: URL */

  if ((url_parse_mailto(lpenv, NULL, mailto) != -1) && lpenv->to && lpenv->to->mailbox &&
      !mutt_regexlist_match(&UnSubscribedLists, lpenv->to->mailbox) &&
      !mutt_regexlist_match(&UnMailLists, lpenv->to->mailbox) &&
      !mutt_regexlist_match(&UnSubscribedLists, lpenv->to->mailbox))
  {
    struct Buffer err;
    char errbuf[256];
    memset(&err, 0, sizeof(err));
    err.data = errbuf;
    err.dsize = sizeof(errbuf);
    /* mutt_regexlist_add() detects duplicates, so it is safe to
     * try to add here without any checks. */
    mutt_regexlist_add(&MailLists, lpenv->to->mailbox, REG_ICASE, &err);
    mutt_regexlist_add(&SubscribedLists, lpenv->to->mailbox, REG_ICASE, &err);
  }

  mutt_env_free(&lpenv);
}

/**
 * parse_parameters - Parse a list of Parameters
 * @param param Parameter list for the results
 * @param s String to parse
 */
static void parse_parameters(struct ParameterList *param, const char *s)
{
  struct Parameter *new = NULL;
  char buf[1024];
  const char *p = NULL;
  size_t i;

  mutt_debug(LL_DEBUG2, "'%s'\n", s);

  while (*s)
  {
    p = strpbrk(s, "=;");
    if (!p)
    {
      mutt_debug(LL_DEBUG1, "malformed parameter: %s\n", s);
      goto bail;
    }

    /* if we hit a ; now the parameter has no value, just skip it */
    if (*p != ';')
    {
      i = p - s;
      /* remove whitespace from the end of the attribute name */
      while (i > 0 && mutt_str_is_email_wsp(s[i - 1]))
        i--;

      /* the check for the missing parameter token is here so that we can skip
       * over any quoted value that may be present.  */
      if (i == 0)
      {
        mutt_debug(LL_DEBUG1, "missing attribute: %s\n", s);
        new = NULL;
      }
      else
      {
        new = mutt_param_new();
        new->attribute = mutt_str_substr_dup(s, s + i);
      }

      s = mutt_str_skip_email_wsp(p + 1); /* skip over the = */

      if (*s == '"')
      {
        bool state_ascii = true;
        s++;
        for (i = 0; *s && (i < (sizeof(buf) - 1)); i++, s++)
        {
          if (C_AssumedCharset && *C_AssumedCharset)
          {
            /* As iso-2022-* has a character of '"' with non-ascii state,
             * ignore it. */
            if ((*s == 0x1b) && (i < (sizeof(buf) - 2)))
            {
              if ((s[1] == '(') && ((s[2] == 'B') || (s[2] == 'J')))
                state_ascii = true;
              else
                state_ascii = false;
            }
          }
          if (state_ascii && (*s == '"'))
            break;
          if (*s == '\\')
          {
            /* Quote the next character */
            buf[i] = s[1];
            if (!*++s)
              break;
          }
          else
            buf[i] = *s;
        }
        buf[i] = '\0';
        if (*s)
          s++; /* skip over the " */
      }
      else
      {
        for (i = 0; *s && (*s != ' ') && (*s != ';') && (i < (sizeof(buf) - 1)); i++, s++)
          buf[i] = *s;
        buf[i] = '\0';
      }

      /* if the attribute token was missing, 'new' will be NULL */
      if (new)
      {
        new->value = mutt_str_strdup(buf);

        mutt_debug(LL_DEBUG2, "parse_parameter: '%s' = '%s'\n",
                   new->attribute ? new->attribute : "", new->value ? new->value : "");

        /* Add this parameter to the list */
        TAILQ_INSERT_HEAD(param, new, entries);
      }
    }
    else
    {
      mutt_debug(LL_DEBUG1, "parameter with no value: %s\n", s);
      s = p;
    }

    /* Find the next parameter */
    if ((*s != ';') && !(s = strchr(s, ';')))
      break; /* no more parameters */

    do
    {
      /* Move past any leading whitespace. the +1 skips over the semicolon */
      s = mutt_str_skip_email_wsp(s + 1);
    } while (*s == ';'); /* skip empty parameters */
  }

bail:

  rfc2231_decode_parameters(param);
}

/**
 * parse_content_disposition - Parse a content disposition
 * @param s String to parse
 * @param ct Body to save the result
 *
 * e.g. parse a string "inline" and set #DISP_INLINE.
 */
static void parse_content_disposition(const char *s, struct Body *ct)
{
  struct ParameterList parms;
  TAILQ_INIT(&parms);

  if (mutt_str_startswith(s, "inline", CASE_IGNORE))
    ct->disposition = DISP_INLINE;
  else if (mutt_str_startswith(s, "form-data", CASE_IGNORE))
    ct->disposition = DISP_FORM_DATA;
  else
    ct->disposition = DISP_ATTACH;

  /* Check to see if a default filename was given */
  s = strchr(s, ';');
  if (s)
  {
    s = mutt_str_skip_email_wsp(s + 1);
    parse_parameters(&parms, s);
    s = mutt_param_get(&parms, "filename");
    if (s)
      mutt_str_replace(&ct->filename, s);
    s = mutt_param_get(&parms, "name");
    if (s)
      ct->form_name = mutt_str_strdup(s);
    mutt_param_free(&parms);
  }
}

/**
 * parse_references - Parse references from an email header
 * @param head List to receive the references
 * @param s    String to parse
 */
static void parse_references(struct ListHead *head, char *s)
{
  char *m = NULL;
  const char *sp = NULL;

  while ((m = mutt_extract_message_id(s, &sp)))
  {
    mutt_list_insert_head(head, m);
    s = NULL;
  }
}

/**
 * parse_content_language - Read the content's language
 * @param s  Language string
 * @param ct Body of the email
 */
static void parse_content_language(const char *s, struct Body *ct)
{
  if (!s || !ct)
    return;

  mutt_debug(LL_DEBUG2, "RFC8255 >> Content-Language set to %s\n", s);
  ct->language = mutt_str_strdup(s);
}

/**
 * mutt_matches_ignore - Does the string match the ignore list
 * @param s String to check
 * @retval true If string matches
 *
 * Checks Ignore and UnIgnore using mutt_list_match
 */
bool mutt_matches_ignore(const char *s)
{
  return mutt_list_match(s, &Ignore) && !mutt_list_match(s, &UnIgnore);
}

/**
 * mutt_check_mime_type - Check a MIME type string
 * @param s String to check
 * @retval num MIME type, e.g. #TYPE_TEXT
 */
int mutt_check_mime_type(const char *s)
{
  if (mutt_str_strcasecmp("text", s) == 0)
    return TYPE_TEXT;
  else if (mutt_str_strcasecmp("multipart", s) == 0)
    return TYPE_MULTIPART;
#ifdef SUN_ATTACHMENT
  else if (mutt_str_strcasecmp("x-sun-attachment", s) == 0)
    return TYPE_MULTIPART;
#endif
  else if (mutt_str_strcasecmp("application", s) == 0)
    return TYPE_APPLICATION;
  else if (mutt_str_strcasecmp("message", s) == 0)
    return TYPE_MESSAGE;
  else if (mutt_str_strcasecmp("image", s) == 0)
    return TYPE_IMAGE;
  else if (mutt_str_strcasecmp("audio", s) == 0)
    return TYPE_AUDIO;
  else if (mutt_str_strcasecmp("video", s) == 0)
    return TYPE_VIDEO;
  else if (mutt_str_strcasecmp("model", s) == 0)
    return TYPE_MODEL;
  else if (mutt_str_strcasecmp("*", s) == 0)
    return TYPE_ANY;
  else if (mutt_str_strcasecmp(".*", s) == 0)
    return TYPE_ANY;
  else
    return TYPE_OTHER;
}

/**
 * mutt_extract_message_id - Find a message-id
 * @param[in]  s String to parse
 * @param[out] saveptr Save result here
 * @retval ptr  First character after message-id
 * @retval NULL No more message ids
 *
 * Extract the first substring that looks like a message-id.
 * Call back with NULL for more (like strtok).
 */
char *mutt_extract_message_id(const char *s, const char **saveptr)
{
  const char *o = NULL, *onull = NULL, *p = NULL;
  char *ret = NULL;

  if (s)
    p = s;
  else if (saveptr)
    p = *saveptr;
  else
    return NULL;

  for (s = NULL, o = NULL, onull = NULL; (p = strpbrk(p, "<> \t;")); p++)
  {
    if (*p == '<')
    {
      s = p;
      o = NULL;
      onull = NULL;
      continue;
    }

    if (!s)
      continue;

    if (*p == '>')
    {
      size_t olen = onull - o;
      size_t slen = p - s + 1;
      ret = mutt_mem_malloc(olen + slen + 1);
      if (o)
        memcpy(ret, o, olen);
      memcpy(ret + olen, s, slen);
      ret[olen + slen] = '\0';
      if (saveptr)
        *saveptr = p + 1; /* next call starts after '>' */
      return ret;
    }

    /* some idiotic clients break their message-ids between lines */
    if (s == p)
    {
      /* step past another whitespace */
      s = p + 1;
    }
    else if (o)
    {
      /* more than two lines, give up */
      s = NULL;
      o = NULL;
      onull = NULL;
    }
    else
    {
      /* remember the first line, start looking for the second */
      o = s;
      onull = p;
      s = p + 1;
    }
  }

  return NULL;
}

/**
 * mutt_check_encoding - Check the encoding type
 * @param c String to check
 * @retval num Encoding type, e.g. #ENC_QUOTED_PRINTABLE
 */
int mutt_check_encoding(const char *c)
{
  if (mutt_str_startswith(c, "7bit", CASE_IGNORE))
    return ENC_7BIT;
  else if (mutt_str_startswith(c, "8bit", CASE_IGNORE))
    return ENC_8BIT;
  else if (mutt_str_startswith(c, "binary", CASE_IGNORE))
    return ENC_BINARY;
  else if (mutt_str_startswith(c, "quoted-printable", CASE_IGNORE))
    return ENC_QUOTED_PRINTABLE;
  else if (mutt_str_startswith(c, "base64", CASE_IGNORE))
    return ENC_BASE64;
  else if (mutt_str_startswith(c, "x-uuencode", CASE_IGNORE))
    return ENC_UUENCODED;
#ifdef SUN_ATTACHMENT
  else if (mutt_str_startswith(c, "uuencode", CASE_IGNORE))
    return ENC_UUENCODED;
#endif
  else
    return ENC_OTHER;
}

/**
 * mutt_parse_content_type - Parse a content type
 * @param s String to parse
 * @param ct Body to save the result
 *
 * e.g. parse a string "inline" and set #DISP_INLINE.
 */
void mutt_parse_content_type(const char *s, struct Body *ct)
{
  FREE(&ct->subtype);
  mutt_param_free(&ct->parameter);

  /* First extract any existing parameters */
  char *pc = strchr(s, ';');
  if (pc)
  {
    *pc++ = 0;
    while (*pc && ISSPACE(*pc))
      pc++;
    parse_parameters(&ct->parameter, pc);

    /* Some pre-RFC1521 gateways still use the "name=filename" convention,
     * but if a filename has already been set in the content-disposition,
     * let that take precedence, and don't set it here */
    pc = mutt_param_get(&ct->parameter, "name");
    if (pc && !ct->filename)
      ct->filename = mutt_str_strdup(pc);

#ifdef SUN_ATTACHMENT
    /* this is deep and utter perversion */
    pc = mutt_param_get(&ct->parameter, "conversions");
    if (pc)
      ct->encoding = mutt_check_encoding(pc);
#endif
  }

  /* Now get the subtype */
  char *subtype = strchr(s, '/');
  if (subtype)
  {
    *subtype++ = '\0';
    for (pc = subtype; *pc && !ISSPACE(*pc) && (*pc != ';'); pc++)
      ;
    *pc = '\0';
    ct->subtype = mutt_str_strdup(subtype);
  }

  /* Finally, get the major type */
  ct->type = mutt_check_mime_type(s);

#ifdef SUN_ATTACHMENT
  if (mutt_str_strcasecmp("x-sun-attachment", s) == 0)
    ct->subtype = mutt_str_strdup("x-sun-attachment");
#endif

  if (ct->type == TYPE_OTHER)
  {
    ct->xtype = mutt_str_strdup(s);
  }

  if (!ct->subtype)
  {
    /* Some older non-MIME mailers (i.e., mailtool, elm) have a content-type
     * field, so we can attempt to convert the type to Body here.  */
    if (ct->type == TYPE_TEXT)
      ct->subtype = mutt_str_strdup("plain");
    else if (ct->type == TYPE_AUDIO)
      ct->subtype = mutt_str_strdup("basic");
    else if (ct->type == TYPE_MESSAGE)
      ct->subtype = mutt_str_strdup("rfc822");
    else if (ct->type == TYPE_OTHER)
    {
      char buf[128];

      ct->type = TYPE_APPLICATION;
      snprintf(buf, sizeof(buf), "x-%s", s);
      ct->subtype = mutt_str_strdup(buf);
    }
    else
      ct->subtype = mutt_str_strdup("x-unknown");
  }

  /* Default character set for text types. */
  if (ct->type == TYPE_TEXT)
  {
    pc = mutt_param_get(&ct->parameter, "charset");
    if (!pc)
    {
      mutt_param_set(&ct->parameter, "charset",
                     (C_AssumedCharset && *C_AssumedCharset) ?
                         (const char *) mutt_ch_get_default_charset() :
                         "us-ascii");
    }
  }
}

/**
 * mutt_rfc822_parse_line - Parse an email header
 * @param env       Envelope of the email
 * @param e         Email
 * @param line      Header field, env.g. 'to'
 * @param p         Header value, env.g. 'john@example.com'
 * @param user_hdrs If true, save into the Envelope's userhdrs
 * @param weed      If true, perform header weeding (filtering)
 * @param do_2047   If true, perform RFC2047 decoding of the field
 * @retval 1 If the field is recognised
 * @retval 0 If not
 *
 * Process a line from an email header.  Each line that is recognised is parsed
 * and the information put in the Envelope or Header.
 */
int mutt_rfc822_parse_line(struct Envelope *env, struct Email *e, char *line,
                           char *p, bool user_hdrs, bool weed, bool do_2047)
{
  bool matched = false;

  switch (tolower(line[0]))
  {
    case 'a':
      if (mutt_str_strcasecmp(line + 1, "pparently-to") == 0)
      {
        env->to = mutt_addr_parse_list(env->to, p);
        matched = true;
      }
      else if (mutt_str_strcasecmp(line + 1, "pparently-from") == 0)
      {
        env->from = mutt_addr_parse_list(env->from, p);
        matched = true;
      }
      break;

    case 'b':
      if (mutt_str_strcasecmp(line + 1, "cc") == 0)
      {
        env->bcc = mutt_addr_parse_list(env->bcc, p);
        matched = true;
      }
      break;

    case 'c':
      if (mutt_str_strcasecmp(line + 1, "c") == 0)
      {
        env->cc = mutt_addr_parse_list(env->cc, p);
        matched = true;
      }
      else
      {
        size_t plen = mutt_str_startswith(line + 1, "ontent-", CASE_IGNORE);
        if (plen != 0)
        {
          if (mutt_str_strcasecmp(line + 1 + plen, "type") == 0)
          {
            if (e)
              mutt_parse_content_type(p, e->content);
            matched = true;
          }
          else if (mutt_str_strcasecmp(line + 1 + plen, "language") == 0)
          {
            if (e)
              parse_content_language(p, e->content);
            matched = true;
          }
          else if (mutt_str_strcasecmp(line + 1 + plen, "transfer-encoding") == 0)
          {
            if (e)
              e->content->encoding = mutt_check_encoding(p);
            matched = true;
          }
          else if (mutt_str_strcasecmp(line + 1 + plen, "length") == 0)
          {
            if (e)
            {
              e->content->length = atol(p);
              if (e->content->length < 0)
                e->content->length = -1;
            }
            matched = true;
          }
          else if (mutt_str_strcasecmp(line + 1 + plen, "description") == 0)
          {
            if (e)
            {
              mutt_str_replace(&e->content->description, p);
              rfc2047_decode(&e->content->description);
            }
            matched = true;
          }
          else if (mutt_str_strcasecmp(line + 1 + plen, "disposition") == 0)
          {
            if (e)
              parse_content_disposition(p, e->content);
            matched = true;
          }
        }
      }
      break;

    case 'd':
      if (mutt_str_strcasecmp("ate", line + 1) == 0)
      {
        mutt_str_replace(&env->date, p);
        if (e)
        {
          struct Tz tz;
          e->date_sent = mutt_date_parse_date(p, &tz);
          if (e->date_sent > 0)
          {
            e->zhours = tz.zhours;
            e->zminutes = tz.zminutes;
            e->zoccident = tz.zoccident;
          }
        }
        matched = true;
      }
      break;

    case 'e':
      if ((mutt_str_strcasecmp("xpires", line + 1) == 0) && e &&
          (mutt_date_parse_date(p, NULL) < time(NULL)))
      {
        e->expired = true;
      }
      break;

    case 'f':
      if (mutt_str_strcasecmp("rom", line + 1) == 0)
      {
        env->from = mutt_addr_parse_list(env->from, p);
        matched = true;
      }
#ifdef USE_NNTP
      else if (mutt_str_strcasecmp(line + 1, "ollowup-to") == 0)
      {
        if (!env->followup_to)
        {
          mutt_str_remove_trailing_ws(p);
          env->followup_to = mutt_str_strdup(mutt_str_skip_whitespace(p));
        }
        matched = true;
      }
#endif
      break;

    case 'i':
      if (mutt_str_strcasecmp(line + 1, "n-reply-to") == 0)
      {
        mutt_list_free(&env->in_reply_to);
        parse_references(&env->in_reply_to, p);
        matched = true;
      }
      break;

    case 'l':
      if (mutt_str_strcasecmp(line + 1, "ines") == 0)
      {
        if (e)
        {
          /* HACK - neomutt has, for a very short time, produced negative
           * Lines header values.  Ignore them.  */
          if ((mutt_str_atoi(p, &e->lines) < 0) || (e->lines < 0))
            e->lines = 0;
        }

        matched = true;
      }
      else if (mutt_str_strcasecmp(line + 1, "ist-Post") == 0)
      {
        /* RFC2369.  FIXME: We should ignore whitespace, but don't. */
        if (strncmp(p, "NO", 2) != 0)
        {
          char *beg = NULL, *end = NULL;
          for (beg = strchr(p, '<'); beg; beg = strchr(end, ','))
          {
            beg++;
            end = strchr(beg, '>');
            if (!end)
              break;

            /* Take the first mailto URL */
            if (url_check_scheme(beg) == U_MAILTO)
            {
              FREE(&env->list_post);
              env->list_post = mutt_str_substr_dup(beg, end);
              if (C_AutoSubscribe)
                mutt_auto_subscribe(env->list_post);

              break;
            }
          }
        }
        matched = true;
      }
      break;

    case 'm':
      if (mutt_str_strcasecmp(line + 1, "ime-version") == 0)
      {
        if (e)
          e->mime = true;
        matched = true;
      }
      else if (mutt_str_strcasecmp(line + 1, "essage-id") == 0)
      {
        /* We add a new "Message-ID:" when building a message */
        FREE(&env->message_id);
        env->message_id = mutt_extract_message_id(p, NULL);
        matched = true;
      }
      else
      {
        size_t plen = mutt_str_startswith(line + 1, "ail-", CASE_IGNORE);
        if (plen != 0)
        {
          if (mutt_str_strcasecmp(line + 1 + plen, "reply-to") == 0)
          {
            /* override the Reply-To: field */
            mutt_addr_free(&env->reply_to);
            env->reply_to = mutt_addr_parse_list(env->reply_to, p);
            matched = true;
          }
          else if (mutt_str_strcasecmp(line + 1 + plen, "followup-to") == 0)
          {
            env->mail_followup_to = mutt_addr_parse_list(env->mail_followup_to, p);
            matched = true;
          }
        }
      }
      break;

#ifdef USE_NNTP
    case 'n':
      if (mutt_str_strcasecmp(line + 1, "ewsgroups") == 0)
      {
        FREE(&env->newsgroups);
        mutt_str_remove_trailing_ws(p);
        env->newsgroups = mutt_str_strdup(mutt_str_skip_whitespace(p));
        matched = true;
      }
      break;
#endif

    case 'o':
      /* field 'Organization:' saves only for pager! */
      if (mutt_str_strcasecmp(line + 1, "rganization") == 0)
      {
        if (!env->organization && (mutt_str_strcasecmp(p, "unknown") != 0))
          env->organization = mutt_str_strdup(p);
      }
      break;

    case 'r':
      if (mutt_str_strcasecmp(line + 1, "eferences") == 0)
      {
        mutt_list_free(&env->references);
        parse_references(&env->references, p);
        matched = true;
      }
      else if (mutt_str_strcasecmp(line + 1, "eply-to") == 0)
      {
        env->reply_to = mutt_addr_parse_list(env->reply_to, p);
        matched = true;
      }
      else if (mutt_str_strcasecmp(line + 1, "eturn-path") == 0)
      {
        env->return_path = mutt_addr_parse_list(env->return_path, p);
        matched = true;
      }
      else if (mutt_str_strcasecmp(line + 1, "eceived") == 0)
      {
        if (e && !e->received)
        {
          char *d = strrchr(p, ';');

          if (d)
            e->received = mutt_date_parse_date(d + 1, NULL);
        }
      }
      break;

    case 's':
      if (mutt_str_strcasecmp(line + 1, "ubject") == 0)
      {
        if (!env->subject)
          env->subject = mutt_str_strdup(p);
        matched = true;
      }
      else if (mutt_str_strcasecmp(line + 1, "ender") == 0)
      {
        env->sender = mutt_addr_parse_list(env->sender, p);
        matched = true;
      }
      else if (mutt_str_strcasecmp(line + 1, "tatus") == 0)
      {
        if (e)
        {
          while (*p)
          {
            switch (*p)
            {
              case 'O':
                e->old = C_MarkOld ? true : false;
                break;
              case 'R':
                e->read = true;
                break;
              case 'r':
                e->replied = true;
                break;
            }
            p++;
          }
        }
        matched = true;
      }
      else if (((mutt_str_strcasecmp("upersedes", line + 1) == 0) ||
                (mutt_str_strcasecmp("upercedes", line + 1) == 0)) &&
               e)
      {
        FREE(&env->supersedes);
        env->supersedes = mutt_str_strdup(p);
      }
      break;

    case 't':
      if (mutt_str_strcasecmp(line + 1, "o") == 0)
      {
        env->to = mutt_addr_parse_list(env->to, p);
        matched = true;
      }
      break;

    case 'x':
      if (mutt_str_strcasecmp(line + 1, "-status") == 0)
      {
        if (e)
        {
          while (*p)
          {
            switch (*p)
            {
              case 'A':
                e->replied = true;
                break;
              case 'D':
                e->deleted = true;
                break;
              case 'F':
                e->flagged = true;
                break;
              default:
                break;
            }
            p++;
          }
        }
        matched = true;
      }
      else if (mutt_str_strcasecmp(line + 1, "-label") == 0)
      {
        FREE(&env->x_label);
        env->x_label = mutt_str_strdup(p);
        matched = true;
      }
#ifdef USE_NNTP
      else if (mutt_str_strcasecmp(line + 1, "-comment-to") == 0)
      {
        if (!env->x_comment_to)
          env->x_comment_to = mutt_str_strdup(p);
        matched = true;
      }
      else if (mutt_str_strcasecmp(line + 1, "ref") == 0)
      {
        if (!env->xref)
          env->xref = mutt_str_strdup(p);
        matched = true;
      }
#endif
      else if (mutt_str_strcasecmp(line + 1, "-original-to") == 0)
      {
        env->x_original_to = mutt_addr_parse_list(env->x_original_to, p);
        matched = true;
      }

    default:
      break;
  }

  /* Keep track of the user-defined headers */
  if (!matched && user_hdrs)
  {
    /* restore the original line */
    line[strlen(line)] = ':';

    if (!(weed && C_Weed && mutt_matches_ignore(line)))
    {
      struct ListNode *np = mutt_list_insert_tail(&env->userhdrs, mutt_str_strdup(line));
      if (do_2047)
        rfc2047_decode(&np->data);
    }
  }

  return matched;
}

/**
 * mutt_rfc822_read_line - Read a header line from a file
 * @param fp      File to read from
 * @param line    Buffer to store the result
 * @param linelen Length of buffer
 * @retval ptr Line read from file
 *
 * Reads an arbitrarily long header field, and looks ahead for continuation
 * lines.  "line" must point to a dynamically allocated string; it is
 * increased if more space is required to fit the whole line.
 */
char *mutt_rfc822_read_line(FILE *fp, char *line, size_t *linelen)
{
  char *buf = line;
  int ch;
  size_t offset = 0;

  while (true)
  {
    if (!fgets(buf, *linelen - offset, fp) || /* end of file or */
        (ISSPACE(*line) && !offset))          /* end of headers */
    {
      *line = '\0';
      return line;
    }

    const size_t len = mutt_str_strlen(buf);
    if (!len)
      return line;

    buf += len - 1;
    if (*buf == '\n')
    {
      /* we did get a full line. remove trailing space */
      while (ISSPACE(*buf))
      {
        *buf-- = '\0'; /* we cannot come beyond line's beginning because
                        * it begins with a non-space */
      }

      /* check to see if the next line is a continuation line */
      ch = fgetc(fp);
      if ((ch != ' ') && (ch != '\t'))
      {
        ungetc(ch, fp);
        return line; /* next line is a separate header field or EOH */
      }

      /* eat tabs and spaces from the beginning of the continuation line */
      while (((ch = fgetc(fp)) == ' ') || (ch == '\t'))
        ;
      ungetc(ch, fp);
      *++buf = ' '; /* string is still terminated because we removed
                       at least one whitespace char above */
    }

    buf++;
    offset = buf - line;
    if (*linelen < (offset + 256))
    {
      /* grow the buffer */
      *linelen += 256;
      mutt_mem_realloc(&line, *linelen);
      buf = line + offset;
    }
  }
  /* not reached */
}

/**
 * mutt_rfc822_read_header - parses an RFC822 header
 * @param fp        Stream to read from
 * @param e         Current Email (optional)
 * @param user_hdrs If set, store user headers
 *                  Used for recall-message and postpone modes
 * @param weed      If this parameter is set and the user has activated the
 *                  $weed option, honor the header weed list for user headers.
 *                  Used for recall-message
 * @retval ptr Newly allocated envelope structure
 *
 * Caller should free the Envelope using mutt_env_free().
 */
struct Envelope *mutt_rfc822_read_header(FILE *fp, struct Email *e, bool user_hdrs, bool weed)
{
  struct Envelope *env = mutt_env_new();
  char *p = NULL;
  LOFF_T loc;
  size_t linelen = 1024;
  char *line = mutt_mem_malloc(linelen);
  char buf[linelen + 1];

  if (e)
  {
    if (!e->content)
    {
      e->content = mutt_body_new();

      /* set the defaults from RFC1521 */
      e->content->type = TYPE_TEXT;
      e->content->subtype = mutt_str_strdup("plain");
      e->content->encoding = ENC_7BIT;
      e->content->length = -1;

      /* RFC2183 says this is arbitrary */
      e->content->disposition = DISP_INLINE;
    }
  }

  while ((loc = ftello(fp)) != -1)
  {
    line = mutt_rfc822_read_line(fp, line, &linelen);
    if (*line == '\0')
      break;
    p = strpbrk(line, ": \t");
    if (!p || (*p != ':'))
    {
      char return_path[1024];
      time_t t;

      /* some bogus MTAs will quote the original "From " line */
      if (mutt_str_startswith(line, ">From ", CASE_MATCH))
        continue; /* just ignore */
      else if (is_from(line, return_path, sizeof(return_path), &t))
      {
        /* MH sometimes has the From_ line in the middle of the header! */
        if (e && !e->received)
          e->received = t - mutt_date_local_tz(t);
        continue;
      }

      fseeko(fp, loc, SEEK_SET);
      break; /* end of header */
    }

    *buf = '\0';

    if (mutt_replacelist_match(&SpamList, buf, sizeof(buf), line))
    {
      if (!mutt_regexlist_match(&NoSpamList, line))
      {
        /* if spam tag already exists, figure out how to amend it */
        if (env->spam && (*buf != '\0'))
        {
          /* If C_SpamSeparator defined, append with separator */
          if (C_SpamSeparator)
          {
            mutt_buffer_addstr(env->spam, C_SpamSeparator);
            mutt_buffer_addstr(env->spam, buf);
          }

          /* else overwrite */
          else
          {
            env->spam->dptr = env->spam->data;
            *env->spam->dptr = '\0';
            mutt_buffer_addstr(env->spam, buf);
          }
        }

        /* spam tag is new, and match expr is non-empty; copy */
        else if (!env->spam && (*buf != '\0'))
        {
          env->spam = mutt_buffer_from(buf);
        }

        /* match expr is empty; plug in null string if no existing tag */
        else if (!env->spam)
        {
          env->spam = mutt_buffer_from("");
        }

        if (env->spam && env->spam->data)
          mutt_debug(5, "spam = %s\n", env->spam->data);
      }
    }

    *p = '\0';
    p = mutt_str_skip_email_wsp(p + 1);
    if (!*p)
      continue; /* skip empty header fields */

    mutt_rfc822_parse_line(env, e, line, p, user_hdrs, weed, true);
  }

  FREE(&line);

  if (e)
  {
    e->content->hdr_offset = e->offset;
    e->content->offset = ftello(fp);

    rfc2047_decode_envelope(env);

    if (env->subject)
    {
      regmatch_t pmatch[1];

      if (C_ReplyRegex && C_ReplyRegex->regex &&
          (regexec(C_ReplyRegex->regex, env->subject, 1, pmatch, 0) == 0))
      {
        env->real_subj = env->subject + pmatch[0].rm_eo;
      }
      else
        env->real_subj = env->subject;
    }

    if (e->received < 0)
    {
      mutt_debug(LL_DEBUG1, "resetting invalid received time to 0\n");
      e->received = 0;
    }

    /* check for missing or invalid date */
    if (e->date_sent <= 0)
    {
      mutt_debug(LL_DEBUG1,
                 "no date found, using received time from msg separator\n");
      e->date_sent = e->received;
    }
  }

  return env;
}

/**
 * mutt_read_mime_header - Parse a MIME header
 * @param fp      stream to read from
 * @param digest  true if reading subparts of a multipart/digest
 * @retval ptr New Body containing parsed structure
 */
struct Body *mutt_read_mime_header(FILE *fp, bool digest)
{
  struct Body *p = mutt_body_new();
  struct Envelope *env = mutt_env_new();
  char *c = NULL;
  size_t linelen = 1024;
  char *line = mutt_mem_malloc(linelen);

  p->hdr_offset = ftello(fp);

  p->encoding = ENC_7BIT; /* default from RFC1521 */
  p->type = digest ? TYPE_MESSAGE : TYPE_TEXT;
  p->disposition = DISP_INLINE;

  while (*(line = mutt_rfc822_read_line(fp, line, &linelen)) != 0)
  {
    /* Find the value of the current header */
    c = strchr(line, ':');
    if (c)
    {
      *c = '\0';
      c = mutt_str_skip_email_wsp(c + 1);
      if (!*c)
      {
        mutt_debug(LL_DEBUG1, "skipping empty header field: %s\n", line);
        continue;
      }
    }
    else
    {
      mutt_debug(LL_DEBUG1, "bogus MIME header: %s\n", line);
      break;
    }

    size_t plen = mutt_str_startswith(line, "content-", CASE_IGNORE);
    if (plen != 0)
    {
      if (mutt_str_strcasecmp("type", line + plen) == 0)
        mutt_parse_content_type(c, p);
      else if (mutt_str_strcasecmp("language", line + plen) == 0)
        parse_content_language(c, p);
      else if (mutt_str_strcasecmp("transfer-encoding", line + plen) == 0)
        p->encoding = mutt_check_encoding(c);
      else if (mutt_str_strcasecmp("disposition", line + plen) == 0)
        parse_content_disposition(c, p);
      else if (mutt_str_strcasecmp("description", line + plen) == 0)
      {
        mutt_str_replace(&p->description, c);
        rfc2047_decode(&p->description);
      }
    }
#ifdef SUN_ATTACHMENT
    else if ((plen = mutt_str_startswith(line, "x-sun-", CASE_IGNORE)))
    {
      if (mutt_str_strcasecmp("data-type", line + plen) == 0)
        mutt_parse_content_type(c, p);
      else if (mutt_str_strcasecmp("encoding-info", line + plen) == 0)
        p->encoding = mutt_check_encoding(c);
      else if (mutt_str_strcasecmp("content-lines", line + plen) == 0)
        mutt_param_set(&p->parameter, "content-lines", c);
      else if (mutt_str_strcasecmp("data-description", line + plen) == 0)
      {
        mutt_str_replace(&p->description, c);
        rfc2047_decode(&p->description);
      }
    }
#endif
    else
    {
      if (mutt_rfc822_parse_line(env, NULL, line, c, false, false, false))
        p->mime_headers = env;
    }
  }
  p->offset = ftello(fp); /* Mark the start of the real data */
  if ((p->type == TYPE_TEXT) && !p->subtype)
    p->subtype = mutt_str_strdup("plain");
  else if ((p->type == TYPE_MESSAGE) && !p->subtype)
    p->subtype = mutt_str_strdup("rfc822");

  FREE(&line);

  if (p->mime_headers)
    rfc2047_decode_envelope(p->mime_headers);
  else
    mutt_env_free(&env);

  return p;
}

/**
 * mutt_is_message_type - Determine if a mime type matches a message or not
 * @param type    Message type enum value
 * @param subtype Message subtype
 * @retval true  Type is message/news or message/rfc822
 * @retval false Otherwise
 */
bool mutt_is_message_type(int type, const char *subtype)
{
  if (type != TYPE_MESSAGE)
    return false;

  subtype = NONULL(subtype);
  return (mutt_str_strcasecmp(subtype, "rfc822") == 0) ||
         (mutt_str_strcasecmp(subtype, "news") == 0);
}

/**
 * mutt_parse_part - Parse a MIME part
 * @param fp File to read from
 * @param b  Body to store the results in
 */
void mutt_parse_part(FILE *fp, struct Body *b)
{
  const char *bound = NULL;

  switch (b->type)
  {
    case TYPE_MULTIPART:
#ifdef SUN_ATTACHMENT
      if (mutt_str_strcasecmp(b->subtype, "x-sun-attachment") == 0)
        bound = "--------";
      else
#endif
        bound = mutt_param_get(&b->parameter, "boundary");

      fseeko(fp, b->offset, SEEK_SET);
      b->parts = mutt_parse_multipart(fp, bound, b->offset + b->length,
                                      (mutt_str_strcasecmp("digest", b->subtype) == 0));
      break;

    case TYPE_MESSAGE:
      if (b->subtype)
      {
        fseeko(fp, b->offset, SEEK_SET);
        if (mutt_is_message_type(b->type, b->subtype))
          b->parts = mutt_rfc822_parse_message(fp, b);
        else if (mutt_str_strcasecmp(b->subtype, "external-body") == 0)
          b->parts = mutt_read_mime_header(fp, 0);
        else
          return;
      }
      break;

    default:
      return;
  }

  /* try to recover from parsing error */
  if (!b->parts)
  {
    b->type = TYPE_TEXT;
    mutt_str_replace(&b->subtype, "plain");
  }
}

/**
 * mutt_parse_multipart - parse a multipart structure
 * @param fp       stream to read from
 * @param boundary body separator
 * @param end_off  length of the multipart body (used when the final
 *                 boundary is missing to avoid reading too far)
 * @param digest   true if reading a multipart/digest
 * @retval ptr New Body containing parsed structure
 */
struct Body *mutt_parse_multipart(FILE *fp, const char *boundary, LOFF_T end_off, bool digest)
{
  char buf[1024];
  struct Body *head = NULL, *last = NULL, *new = NULL;
  bool final = false; /* did we see the ending boundary? */

  if (!boundary)
  {
    mutt_error(_("multipart message has no boundary parameter"));
    return NULL;
  }

  const size_t blen = mutt_str_strlen(boundary);
  while ((ftello(fp) < end_off) && fgets(buf, sizeof(buf), fp))
  {
    const size_t len = mutt_str_strlen(buf);

    const size_t crlf = ((len > 1) && (buf[len - 2] == '\r')) ? 1 : 0;

    if ((buf[0] == '-') && (buf[1] == '-') && mutt_str_startswith(buf + 2, boundary, CASE_MATCH))
    {
      if (last)
      {
        last->length = ftello(fp) - last->offset - len - 1 - crlf;
        if (last->parts && (last->parts->length == 0))
          last->parts->length = ftello(fp) - last->parts->offset - len - 1 - crlf;
        /* if the body is empty, we can end up with a -1 length */
        if (last->length < 0)
          last->length = 0;
      }

      if (len > 0)
      {
        /* Remove any trailing whitespace, up to the length of the boundary */
        for (size_t i = len - 1; ISSPACE(buf[i]) && (i >= (blen + 2)); i--)
          buf[i] = '\0';
      }

      /* Check for the end boundary */
      if (mutt_str_strcmp(buf + blen + 2, "--") == 0)
      {
        final = true;
        break; /* done parsing */
      }
      else if (buf[2 + blen] == '\0')
      {
        new = mutt_read_mime_header(fp, digest);

#ifdef SUN_ATTACHMENT
        if (mutt_param_get(&new->parameter, "content-lines"))
        {
          int lines;
          if (mutt_str_atoi(mutt_param_get(&new->parameter, "content-lines"), &lines) < 0)
            lines = 0;
          for (; lines; lines--)
            if ((ftello(fp) >= end_off) || !fgets(buf, sizeof(buf), fp))
              break;
        }
#endif
        /* Consistency checking - catch bad attachment end boundaries */
        if (new->offset > end_off)
        {
          mutt_body_free(&new);
          break;
        }
        if (head)
        {
          last->next = new;
          last = new;
        }
        else
        {
          last = new;
          head = new;
        }
      }
    }
  }

  /* in case of missing end boundary, set the length to something reasonable */
  if (last && (last->length == 0) && !final)
    last->length = end_off - last->offset;

  /* parse recursive MIME parts */
  for (last = head; last; last = last->next)
    mutt_parse_part(fp, last);

  return head;
}

/**
 * mutt_rfc822_parse_message - parse a Message/RFC822 body
 * @param fp     stream to read from
 * @param parent info about the message/rfc822 body part
 * @retval ptr New Body containing parsed message
 *
 * NOTE: this assumes that 'parent->length' has been set!
 */
struct Body *mutt_rfc822_parse_message(FILE *fp, struct Body *parent)
{
  parent->email = mutt_email_new();
  parent->email->offset = ftello(fp);
  parent->email->env = mutt_rfc822_read_header(fp, parent->email, false, false);
  struct Body *msg = parent->email->content;

  /* ignore the length given in the content-length since it could be wrong
   * and we already have the info to calculate the correct length */
  /* if (msg->length == -1) */
  msg->length = parent->length - (msg->offset - parent->offset);

  /* if body of this message is empty, we can end up with a negative length */
  if (msg->length < 0)
    msg->length = 0;

  mutt_parse_part(fp, msg);
  return msg;
}
