#include "ajc_c.h"
#include "test.h"

TEST_DEFINE(test_add_vargs)
{
  ASSERT_EQ(ajc_add_vargs_c(1, 42), 42);
  ASSERT_EQ(ajc_add_vargs_c(2, 2, 2), 4);
  ASSERT_EQ(ajc_add_vargs_c(3, 2, 3, 4), 9);
}

RUN_TESTS(test_add_vargs)
