#pragma once

#include "macros.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef void (*test_fn_t)(void);

typedef struct {
  const char *names;
  const test_fn_t *test_fns;
  size_t n_fns;
} test_manifest_t;

#define TEST_DEFINE(name) static void name(void)

#define RUN_TESTS(...)\
  static const test_fn_t test_fns_[] = { __VA_ARGS__ }; \
  static const test_manifest_t test_manifest_ = { \
    #__VA_ARGS__, \
    test_fns_, \
    ARRAY_LENGTH(test_fns_), \
  }; \
  int main(void) { return test_main(&test_manifest_); }

#define TEST_FAIL(fmt, ...)\
  do { \
    test_fail("Assertion failed [%s:%d] " fmt, \
        __FILE__, __LINE__, ##__VA_ARGS__); \
    return; \
  } while(0)

#define ASSERT_OP_I(e1, e2, op, ty, fmt)\
  do { \
    ty v1 = (e1); \
    ty v2 = (e2); \
    if (v1 op v2) { \
      TEST_FAIL(#e1 " " #op " " #e2 ": " fmt " vs " fmt, \
          v1, v2); \
    } \
  } while(0)

#define ASSERT_EQ_I(e1, e2, ty, fmt)\
  ASSERT_OP_I(e1, e2, !=, ty, fmt)

#define ASSERT_NEQ_I(e1, e2, ty, fmt)\
  ASSERT_OP_I(e1, e2, ==, ty, fmt)

#define ASSERT_EQ(e1, e2)\
  ASSERT_EQ_I(e1, e2, int, "%d")

#define ASSERT_EQ_X(e1, e2)\
  ASSERT_EQ_I(e1, e2, int, "0x%x")

#define ASSERT_THAT(e1)\
  ASSERT_EQ(!(e1), 0)

#define ASSERT_PTR_EQ(e1, e2)\
  ASSERT_EQ_I(e1, e2, void *, "%p")

#define ASSERT_PTR_NEQ(e1, e2)\
  ASSERT_NEQ_I(e1, e2, void *, "%p")

#define ASSERT_STR_EQ(e_x, e_y) \
  do { \
    const char *x = (e_x); \
    const char *y = (e_y); \
    if (strcmp(x, y)) { \
      TEST_FAIL(#e_x " != " #e_y ": '%s' vs '%s'", \
          x, y); \
    } \
  } while (0)

void test_fail(const char *fmt, ...);

int test_main(const test_manifest_t *data);

typedef enum {
  ERROR_SUCCESS = 0,
  ERROR_FAILURE = 1,
  ERROR_TRY_AGAIN = 2,
} error_code_t;

typedef struct {
  int pipe[2];
} fd_capture_t;

void fd_capture_open(fd_capture_t *cap, int to_capture);

error_code_t fd_capture_read(fd_capture_t *cap,
    void *buf, size_t buflen, size_t *bytes_read);
