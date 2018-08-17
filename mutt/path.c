/**
 * @file
 * Path manipulation functions
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
 * @page path Path manipulation functions
 *
 * Path manipulation functions
 */

#include "config.h"
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mutt/logging.h"
#include "mutt/memory.h"
#include "mutt/message.h"
#include "mutt/string2.h"

/**
 * mutt_path_tidy_slash - Remove unnecessary slashes and dots
 * @param[in,out] buf Path to modify
 * @retval true Success
 *
 * Collapse repeated '//' and '/./'
 */
bool mutt_path_tidy_slash(char *buf)
{
  if (!buf)
    return false;

  char *r = buf;
  char *w = buf;

  while (*r != '\0')
  {
    *w++ = *r++;

    if (r[-1] == '/') /* After a '/' ... */
    {
      for (; (r[0] == '/') || (r[0] == '.'); r++)
      {
        if (r[0] == '/') /* skip multiple /s */
          continue;
        if (r[0] == '.')
        {
          if (r[1] == '/') /* skip /./ */
          {
            r++;
            continue;
          }
          else if (r[1] == '\0') /* skip /.$ */
          {
            r[0] = '\0';
          }
          break; /* dot-anything-else isn't special */
        }
      }
    }
  }

  /* Strip a trailing / as long as it's not the only character */
  if ((w > (buf + 1)) && (w[-1] == '/'))
    w--;

  *w = '\0';

  return true;
}

/**
 * mutt_path_tidy_dotdot - Remove dot-dot-slash from a path
 * @param[in,out] buf Path to modify
 * @retval true Success
 *
 * Collapse dot-dot patterns, like '/dir/../'
 */
bool mutt_path_tidy_dotdot(char *buf)
{
  if (!buf || (buf[0] != '/'))
    return false;

  char *dd = buf;

  mutt_debug(3, "Collapse path: %s\n", buf);
  while ((dd = strstr(dd, "/..")))
  {
    if (dd[3] == '/') /* paths follow dots */
    {
      char *dest = NULL;
      if (dd != buf) /* not at start of string */
      {
        dd[0] = '\0';
        dest = strrchr(buf, '/');
      }
      if (!dest)
        dest = buf;

      memmove(dest, dd + 3, strlen(dd + 3) + 1);
    }
    else if (dd[3] == '\0') /* dots at end of string */
    {
      if (dd == buf) /* at start of string */
      {
        dd[1] = '\0';
      }
      else
      {
        dd[0] = '\0';
        char *s = strrchr(buf, '/');
        if (s == buf)
          s[1] = '\0';
        else if (s)
          s[0] = '\0';
      }
    }
    else
    {
      dd += 3; /* skip over '/..dir/' */
      continue;
    }

    dd = buf; /* restart at the beginning */
  }

  mutt_debug(3, "Collapsed to:  %s\n", buf);
  return true;
}

/**
 * mutt_path_tidy - Remove unnecessary parts of a path
 * @param[in,out] buf Path to modify
 * @retval true Success
 *
 * Remove unnecessary dots and slashes from a path
 */
bool mutt_path_tidy(char *buf)
{
  if (!buf || (buf[0] != '/'))
    return false;

  if (!mutt_path_tidy_slash(buf))
    return false;

  return mutt_path_tidy_dotdot(buf);
}

/**
 * mutt_path_pretty - Tidy a filesystem path
 * @param buf    Path to modify
 * @param buflen Length of the buffer
 * @param homedir Home directory for '~' substitution
 * @retval true Success
 *
 * Tidy a path and replace a home directory with '~'
 */
bool mutt_path_pretty(char *buf, size_t buflen, const char *homedir)
{
  if (!buf)
    return false;

  mutt_path_tidy(buf);

  size_t len = mutt_str_strlen(homedir);
  if ((len == 0) || (len >= buflen))
    return false;

  char end = buf[len];
  if ((end != '/') && (end != '\0'))
    return false;

  if (mutt_str_strncmp(buf, homedir, len) != 0)
    return false;

  buf[0] = '~';
  if (end == '\0')
  {
    buf[1] = '\0';
    return true;
  }

  mutt_str_strfcpy(buf + 1, buf + len, buflen - len);
  return true;
}

/**
 * mutt_path_canon - Create the canonical version of a path
 * @param buf     Path to modify
 * @param buflen  Length of the buffer
 * @param homedir Home directory for '~' substitution
 * @retval true Success
 *
 * Remove unnecessary dots and slashes from a path and expand shell '~'
 * abbreviations:
 * - ~/dir (~ expanded)
 * - ~realuser/dir (~realuser expanded)
 * - ~nonuser/dir (~nonuser not changed)
 */
bool mutt_path_canon(char *buf, size_t buflen, const char *homedir)
{
  if (!buf || (buf[0] != '~'))
    return false;

  char result[PATH_MAX] = { 0 };
  char *dir = NULL;
  size_t len = 0;

  if ((buf[1] == '/') || (buf[1] == '\0'))
  {
    if (!homedir)
      return false;

    len = mutt_str_strfcpy(result, homedir, sizeof(result));
    dir = buf + 1;
  }
  else
  {
    char user[SHORT_STRING];
    dir = strchr(buf + 1, '/');
    if (dir)
      mutt_str_strfcpy(user, buf + 1, MIN(dir - buf, (unsigned) sizeof(user)));
    else
      mutt_str_strfcpy(user, buf + 1, sizeof(user));

    struct passwd *pw = getpwnam(user);
    if (!pw || !pw->pw_dir)
      return false;

    len = mutt_str_strfcpy(result, pw->pw_dir, sizeof(result));
  }

  len += mutt_str_strfcpy(result + len, dir, sizeof(result) - len);

  if (len >= buflen)
    return false;

  mutt_str_strfcpy(buf, result, buflen);

  if (!mutt_path_tidy(buf))
    return false;

  return true;
}

/**
 * mutt_path_basename - Find the last component for a pathname
 * @param f String to be examined
 * @retval ptr Part of pathname after last '/' character
 */
const char *mutt_path_basename(const char *f)
{
  const char *p = strrchr(f, '/');
  if (p)
    return p + 1;
  else
    return f;
}

/**
 * mutt_path_concat - Join a directory name and a filename
 * @param d     Buffer for the result
 * @param dir   Directory name
 * @param fname File name
 * @param l     Length of buffer
 * @retval ptr Destination buffer
 *
 * If both dir and fname are supplied, they are separated with '/'.
 * If either is missing, then the other will be copied exactly.
 */
char *mutt_path_concat(char *d, const char *dir, const char *fname, size_t l)
{
  const char *fmt = "%s/%s";

  if (!*fname || (*dir && dir[strlen(dir) - 1] == '/'))
    fmt = "%s%s";

  snprintf(d, l, fmt, dir, fname);
  return d;
}

/**
 * mutt_path_concatn - Concatenate directory and filename
 * @param dst      Buffer for result
 * @param dstlen   Buffer length
 * @param dir      Directory
 * @param dirlen   Directory length
 * @param fname    Filename
 * @param fnamelen Filename length
 * @retval NULL Error
 * @retval ptr  Success, pointer to \a dst
 *
 * Write the concatenated pathname (dir + "/" + fname) into dst.
 * The slash is omitted when dir or fname is of 0 length.
 */
char *mutt_path_concatn(char *dst, size_t dstlen, const char *dir,
                             size_t dirlen, const char *fname, size_t fnamelen)
{
  size_t req;
  size_t offset = 0;

  if (dstlen == 0)
    return NULL; /* probably should not mask errors like this */

  /* size check */
  req = dirlen + fnamelen + 1; /* +1 for the trailing nul */
  if (dirlen && fnamelen)
    req++; /* when both components are non-nul, we add a "/" in between */
  if (req > dstlen)
  { /* check for condition where the dst length is too short */
    /* Two options here:
     * 1) assert(0) or return NULL to signal error
     * 2) copy as much of the path as will fit
     * It doesn't appear that the return value is actually checked anywhere mutt_path_concat()
     * is called, so we should just copy set dst to nul and let the calling function fail later.
     */
    dst[0] = '\0'; /* safe since we bail out early if dstlen == 0 */
    return NULL;
  }

  if (dirlen)
  { /* when dir is not empty */
    memcpy(dst, dir, dirlen);
    offset = dirlen;
    if (fnamelen)
      dst[offset++] = '/';
  }
  if (fnamelen)
  { /* when fname is not empty */
    memcpy(dst + offset, fname, fnamelen);
    offset += fnamelen;
  }
  dst[offset] = '\0';
  return dst;
}

/**
 * mutt_path_dirname - Return a path up to, but not including, the final '/'
 * @param  path Path
 * @retval ptr  The directory containing p
 *
 * Unlike the IEEE Std 1003.1-2001 specification of dirname(3), this
 * implementation does not modify its parameter, so callers need not manually
 * copy their paths into a modifiable buffer prior to calling this function.
 */
char *mutt_path_dirname(const char *path)
{
  char buf[PATH_MAX] = { 0 };
  mutt_str_strfcpy(buf, path, sizeof(buf));
  return mutt_str_strdup(dirname(buf));
}

/**
 * mutt_path_to_absolute - Convert relative filepath to an absolute path
 * @param path      Relative path
 * @param reference Absolute path that \a path is relative to
 * @retval true  Success
 * @retval false Failure
 *
 * Use POSIX functions to convert a path to absolute, relatively to another path
 *
 * @note \a path should be at least of PATH_MAX length
 */
int mutt_path_to_absolute(char *path, const char *reference)
{
  char abs_path[PATH_MAX];
  int path_len;

  /* if path is already absolute, don't do anything */
  if ((strlen(path) > 1) && (path[0] == '/'))
  {
    return true;
  }

  char *dirpath = mutt_path_dirname(reference);
  mutt_str_strfcpy(abs_path, dirpath, sizeof(abs_path));
  FREE(&dirpath);
  mutt_str_strncat(abs_path, sizeof(abs_path), "/", 1); /* append a / at the end of the path */

  path_len = sizeof(abs_path) - strlen(path);

  mutt_str_strncat(abs_path, sizeof(abs_path), path, path_len > 0 ? path_len : 0);

  path = realpath(abs_path, path);
  if (!path && (errno != ENOENT))
  {
    mutt_perror(_("Error: converting path to absolute"));
    return false;
  }

  return true;
}

/**
 * mutt_path_realpath - resolve path, unraveling symlinks
 * @param buf Buffer containing path
 * @retval num String length of resolved path
 * @retval 0   Error, buf is not overwritten
 *
 * Resolve and overwrite the path in buf.
 *
 * @note Size of buf should be at least PATH_MAX bytes.
 */
size_t mutt_path_realpath(char *buf)
{
  char s[PATH_MAX];

  if (realpath(buf, s) == NULL)
    return 0;

  return mutt_str_strfcpy(buf, s, sizeof(s));
}

/**
 * mutt_path_get_parent - Find the parent of a path
 * @param path   Path to use
 * @param buf    Buffer for the result
 * @param buflen Length of buffer
 */
void mutt_path_get_parent(char *path, char *buf, size_t buflen)
{
  if (!path || !buf || (buflen == 0))
    return;

  mutt_str_strfcpy(buf, path, buflen);
  int n = mutt_str_strlen(buf);
  if (n == 0)
    return;

  /* remove any final trailing '/' */
  if (buf[n - 1] == '/')
    buf[n - 1] = '\0';

  /* Remove everything until the next slash */
  for (n--; ((n >= 0) && (buf[n] != '/')); n--)
    ;

  if (n > 0)
    buf[n] = '\0';
  else
  {
    buf[0] = '/';
    buf[1] = '\0';
  }
}
