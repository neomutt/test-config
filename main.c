#include <ctype.h>
#include <stdio.h>
#include "test/address.h"
#include "test/bool.h"
#include "test/magic.h"
#include "test/mbyte_table.h"
#include "test/number.h"
#include "test/path.h"
#include "test/quad.h"
#include "test/regex.h"
#include "test/sort.h"
#include "test/string.h"

int main(int argc, char *argv[])
{
  int which = -1;

  if (argc == 2)
    which = argv[1][0] - '0';

  if ((which < 0) || (which == 0)) if (!bool_test())       printf("bool_test() failed\n");
  if ((which < 0) || (which == 1)) if (!number_test())     printf("number_test() failed\n");
  if ((which < 0) || (which == 2)) if (!string_test())     printf("string_test() failed\n");
  if ((which < 0) || (which == 3)) if (!path_test())       printf("path_test() failed\n");
  if ((which < 0) || (which == 4)) if (!quad_test())       printf("quad_test() failed\n");
  if ((which < 0) || (which == 5)) if (!magic_test())      printf("magic_test() failed\n");
  if ((which < 0) || (which == 6)) if (!address_test())    printf("address_test() failed\n");
  if ((which < 0) || (which == 7)) if (!regex_test())      printf("regex_test() failed\n");
  if ((which < 0) || (which == 8)) if (!mbytetable_test()) printf("mbytetable_test() failed\n");
  if ((which < 0) || (which == 9)) if (!sort_test())       printf("sort_test() failed\n");

  return 0;
}

