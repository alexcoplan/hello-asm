#include "test.h"
#include "ajc_c.h"

#include <stdlib.h>
#include <errno.h>

TEST_DEFINE(test_atoi)
{
  int ret = 0;
  ASSERT_EQ(ajc_atoi_c("7", &ret), true);
  ASSERT_EQ(ret, 7);
}

RUN_TESTS(test_atoi)
