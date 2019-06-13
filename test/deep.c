/**
 * @file
 * Test code for Deep Inheritance
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

#include "config.h"
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "mutt/logging.h"
#include "config/lib.h"
#include "account.h"
#include "dump/dump.h"
#include "test/acutest.h"
#include "test/common.h"

static short VarApple;
static short VarBanana;
static short VarCherry;
static short VarDamson;
static short VarElderberry;
static short VarFig;
static short VarGuava;
static short VarHawthorn;
static short VarIlama;

// clang-format off
static struct ConfigDef Vars[] = {
  { "Apple",      DT_NUMBER, &VarApple,      11, },
  { "Banana",     DT_NUMBER, &VarBanana,     22, },
  { "Cherry",     DT_NUMBER, &VarCherry,     33, },
  { "Damson",     DT_NUMBER, &VarDamson,     44, },
  { "Elderberry", DT_NUMBER, &VarElderberry, 55, },
  { "Fig",        DT_NUMBER, &VarFig,        66, },
  { "Guava",      DT_NUMBER, &VarGuava,      77, },
  { "Hawthorn",   DT_NUMBER, &VarHawthorn,   88, },
  { "Ilama",      DT_NUMBER, &VarIlama,      99, },
  { NULL },
};
// clang-format on

struct DeepTest
{
  int b_set;
  int a_set;
  int m_set;
  int b_expected;
  int a_expected;
  int m_expected;
};

static void dump_value(struct ConfigSet *cs, const char *name)
{
  const char *missing = "\033[1;31mX\033[0m";
  const char *inherited = "\033[1;33mI\033[0m";

  struct HashElem *he = cs_get_elem(cs, name);
  if (!he)
  {
    printf("      %s", missing);
    return;
  }

  intptr_t val = cs_he_native_get(cs, he, NULL);
  printf("%6ld", val);

  if (he->type & DT_INHERITED)
    printf("%s", inherited);
  else
    printf(" ");
}

static void dump_values(const char *b_name, struct ConfigSet *cs,
                        struct ConfigSubset *account, struct ConfigSubset *mailbox)
{
  char a_name[128];
  char m_name[128];

  snprintf(a_name, sizeof(a_name), "%s:%s", account->name, b_name);
  snprintf(m_name, sizeof(m_name), "%s:%s", mailbox->name, b_name);

  dump_value(cs, b_name);
  dump_value(cs, a_name);
  dump_value(cs, m_name);

  printf("\n");
}

static void dump_compare(struct ConfigSet *cs, struct HashElem *he, int expected)
{
  intptr_t val = cs_he_native_get(cs, he, NULL);

  printf("%4ld", val);
  if (val == expected)
  {
    printf("\033[1;32m✓\033[0m");
  }
  else
  {
    printf("\033[1;31m✗\033[0m");
    printf(" (%d)", expected);
  }
  printf("  ");
}

static bool test_deep(struct ConfigSet *cs, struct Buffer *err)
{
  bool result = false;

  const char *account_name = "ac";
  const char *account_vars[] = {
    "Apple", "Banana", "Cherry", "Damson", "Elderberry", "Fig", NULL,
  };

  const char *mailbox_name = "ac:mbox";
  const char *mailbox_vars[] = { "Damson", "Elderberry", "Fig", NULL };

  struct ConfigSubset *account_sub = config_subset_new(cs, account_name, NULL, account_vars);
  struct ConfigSubset *mailbox_sub = config_subset_new(cs, mailbox_name, account_name, mailbox_vars);

  printf("             Base  Account Mailbox\n");
  for (size_t i = 0; Vars[i].name; i++)
  {
    printf("%-10s", Vars[i].name);
    dump_values(Vars[i].name, cs, account_sub, mailbox_sub);
  }
  printf("\n");

  // clang-format off
  struct DeepTest dt[] = {
    { -1,  -1,   -1, 55,  55,   55 },
    { -1,  -1, 1234, 55,  55, 1234 },
    { -1, 666,   -1, 55, 666,  666 },
    { -1, 666, 1234, 55, 666, 1234 },
    { 99,  -1,   -1, 99,  99,   99 },
    { 99,  -1, 1234, 99,  99, 1234 },
    { 99, 666,   -1, 99, 666,  666 },
    { 99, 666, 1234, 99, 666, 1234 },
  };
  // clang-format on

  struct HashElem *b_he = cs_get_elem(cs, "Elderberry");
  struct HashElem *a_he = cs_get_elem(cs, "ac:Elderberry");
  struct HashElem *m_he = cs_get_elem(cs, "ac:mbox:Elderberry");

  cs_he_initial_get(cs, b_he, err);

  printf(" I     B    A    M     Base  Account Mailbox\n");
  for (size_t i = 0; i < mutt_array_size(dt); i++)
  {
    cs_he_reset(cs, b_he, NULL);
    cs_he_reset(cs, a_he, NULL);
    cs_he_reset(cs, m_he, NULL);

    if (dt[i].b_set > 0)
      cs_he_native_set(cs, b_he, dt[i].b_set, NULL);
    if (dt[i].a_set > 0)
      cs_he_native_set(cs, a_he, dt[i].a_set, NULL);
    if (dt[i].m_set > 0)
      cs_he_native_set(cs, m_he, dt[i].m_set, NULL);

    printf("%s |", mutt_b2s(err));
    if (dt[i].b_set > 0)
      printf("%4d ", dt[i].b_set);
    else
      printf("   . ");
    if (dt[i].a_set > 0)
      printf("%4d ", dt[i].a_set);
    else
      printf("   . ");
    if (dt[i].m_set > 0)
      printf("%4d ", dt[i].m_set);
    else
      printf("   . ");
    printf(" | ");

    dump_compare(cs, b_he, dt[i].b_expected);
    dump_compare(cs, a_he, dt[i].a_expected);
    dump_compare(cs, m_he, dt[i].m_expected);
    printf("\n");
  }
  printf("\n");

  config_subset_free(&account_sub);
  config_subset_free(&mailbox_sub);

  return result;
}

void config_deep(void)
{
  struct Buffer err;
  mutt_buffer_init(&err);
  err.dsize = 256;
  err.data = mutt_mem_calloc(1, err.dsize);
  mutt_buffer_reset(&err);

  struct ConfigSet *cs = cs_new(30);

  number_init(cs);
  dont_fail = true;
  if (!cs_register_variables(cs, Vars, 0))
    return;
  dont_fail = false;

  // notify_observer_add(cs->notify, NT_CONFIG, 0, log_observer, 0);

  TEST_CHECK(test_deep(cs, &err));

  cs_free(&cs);
  FREE(&err.data);
}
