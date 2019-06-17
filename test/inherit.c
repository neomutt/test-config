/**
 * @file
 * Test code for Inheritance
 *
 * @authors
 * Copyright (C) 2019 Richard Russon <rich@flatcap.org>
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

#define TEST_NO_MAIN
#include "acutest.h"
#include "config.h"
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mutt/mutt.h"
#include "config/lib.h"
#include "email/lib.h"
#include "account.h"
#include "common.h"

static struct Address *VarApple;
static struct Address *VarBanana;

const int VID_AC_APPLE = 0;
const int VID_AC_BANANA = 1;

// clang-format off
static struct ConfigDef Vars[] = {
  { "Apple",  DT_ADDRESS, &VarApple,  0,                       0, NULL },
  { "Banana", DT_ADDRESS, &VarBanana, IP "banana@example.com", 0, NULL },
  { NULL },
};
// clang-format on

bool t_initial(void)
{
  log_line(__func__);

  struct Buffer *err = mutt_buffer_alloc(256);
  struct ConfigSet *cs = cs_new(30);
  address_init(cs);
  if (!cs_register_variables(cs, Vars, 0))
    return false;

  notify_observer_add(cs->notify, NT_CONFIG, 0, log_observer, 0);

  TEST_MSG("Apple  = '%s'\n", VarApple ? VarApple->mailbox : "");
  TEST_MSG("Banana = '%s'\n", VarBanana ? VarBanana->mailbox : "");

  cs_free(&cs);
  mutt_buffer_free(&err);

  log_line(__func__);
  return true;
}

bool t_value(void)
{
  log_line(__func__);

  int rc;
  struct Buffer *err = mutt_buffer_alloc(256);
  struct ConfigSet *cs = cs_new(30);
  address_init(cs);
  if (!cs_register_variables(cs, Vars, 0))
    return false;

  notify_observer_add(cs->notify, NT_CONFIG, 0, log_observer, 0);

  rc = cs_str_string_set(cs, "Apple", "john@example.com", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;

  rc = cs_str_string_set(cs, "Banana", "dave@example.com", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;

  TEST_MSG("Apple  = '%s'\n", VarApple ? VarApple->mailbox : "");
  TEST_MSG("Banana = '%s'\n", VarBanana ? VarBanana->mailbox : "");

  cs_free(&cs);
  mutt_buffer_free(&err);

  log_line(__func__);
  return true;
}

bool t_vid(struct ConfigSubset *sub, const char *name, const char *scope, int vid, struct Buffer *err)
{
  int rc;

  // get a
  mutt_buffer_reset(err);
  rc = cs_subset_string_get(sub, vid, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;
  TEST_MSG("%s:%s = '%s'\n", scope, name, mutt_b2s(err));

  // set n
  rc = cs_str_string_set(sub->cs, name, "john@example.com", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;

  // get a
  mutt_buffer_reset(err);
  rc = cs_subset_string_get(sub, vid, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;
  TEST_MSG("%s:%s = '%s'\n", scope, name, mutt_b2s(err));

  // reset n
  rc = cs_str_reset(sub->cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;

  // get a
  mutt_buffer_reset(err);
  rc = cs_subset_string_get(sub, vid, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;
  TEST_MSG("%s:%s = '%s'\n", scope, name, mutt_b2s(err));

  // set a
  mutt_buffer_reset(err);
  rc = cs_subset_string_set(sub, vid, "jim@gmail.com", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;

  // get a
  mutt_buffer_reset(err);
  rc = cs_subset_string_get(sub, vid, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;
  TEST_MSG("%s:%s = '%s'\n", scope, name, mutt_b2s(err));

  // reset n
  rc = cs_subset_reset(sub, vid, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;

  // get a
  mutt_buffer_reset(err);
  rc = cs_subset_string_get(sub, vid, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
    return false;
  TEST_MSG("%s:%s = '%s'\n", scope, name, mutt_b2s(err));

  return true;
}

bool t_account(void)
{
  log_line(__func__);

  struct Buffer *err = mutt_buffer_alloc(256);
  struct ConfigSet *cs = cs_new(30);
  address_init(cs);
  if (!cs_register_variables(cs, Vars, 0))
    return false;

  const char *account_name = "ac";
  const char *account_vars[] = { "Apple", "Banana", NULL };

  struct ConfigSubset *account_sub = cs_subset_new(cs, account_name, NULL, account_vars);

  notify_observer_add(cs->notify, NT_CONFIG, 0, log_observer, 0);

  set_list(cs);

  if (!t_vid(account_sub, "Apple", "ac", VID_AC_APPLE, err))
    return false;

  if (!t_vid(account_sub, "Banana", "ac", VID_AC_BANANA, err))
    return false;

  cs_subset_free(&account_sub);
  short_line();
  cs_free(&cs);
  mutt_buffer_free(&err);

  log_line(__func__);
  return true;
}

bool t_mailbox(void)
{
  log_line(__func__);

  struct Buffer *err = mutt_buffer_alloc(256);
  struct ConfigSet *cs = cs_new(30);
  address_init(cs);
  if (!cs_register_variables(cs, Vars, 0))
    return false;

  const char *account_name = "ac";
  const char *account_vars[] = { "Apple", "Banana", NULL };

  struct ConfigSubset *account_sub = cs_subset_new(cs, account_name, NULL, account_vars);

  const char *mailbox_name = "mbox";
  const char *mailbox_vars[] = { "Apple", "Banana", NULL };

  struct ConfigSubset *mailbox_sub = cs_subset_new(cs, mailbox_name, account_name, mailbox_vars);

  set_list(cs);

  notify_observer_add(cs->notify, NT_CONFIG, 0, log_observer, 0);

  if (!t_vid(mailbox_sub, "Apple", "ac:mbox", VID_AC_APPLE, err))
    return false;

  if (!t_vid(mailbox_sub, "Banana", "ac:mbox", VID_AC_BANANA, err))
    return false;

  cs_subset_free(&mailbox_sub);
  cs_subset_free(&account_sub);
  cs_free(&cs);
  mutt_buffer_free(&err);

  log_line(__func__);
  return true;
}

void config_inherit(void)
{
  // t_initial();
  // t_value();
  // t_account();
  t_mailbox();
}
