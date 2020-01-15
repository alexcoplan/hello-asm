#include "test.h"
#include "ajc_lib.h"
#include "debug.h"

typedef struct {
  const char *input;
  unsigned ok;
  int val;
} case_t;

static const case_t cases[] = {
#define OK(n) { #n, 1, n }
#define BAD(s) { s, 0, 0 }
  OK(7),
  OK(13),
  OK(10),
  OK(1000),
  OK(-5),
  OK(0),
  BAD("00"),
  OK(13571),
  BAD("cactus"),
  BAD("-"),
  BAD("999999999999999999999999999"),
#undef OK
#undef BAD
};

TEST_DEFINE(test_atoi_asm)
{
  unsigned ok;
  int val;
  for (size_t i = 0; i < ARRAY_LENGTH(cases); i++) {
    const case_t *c = cases + i;
    ok = ajc_atoi(c->input, &val);
    ASSERT_EQ(ok, c->ok);
    if (c->ok) {
      ASSERT_EQ(val, c->val);
    }
  }
}

RUN_TESTS(test_atoi_asm)
