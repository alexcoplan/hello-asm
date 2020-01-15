#include "test.h"
#include "macros.h"
#include "debug.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

static struct {
  bool failed;
  char msg[1024];
} run_state;

int test_main(const test_manifest_t *data)
{
  int ret = 0;

  char names[1024];
  snprintf(names, sizeof(names), "%s", data->names);

  char *tmp = names;

  for (size_t i = 0; i < data->n_fns; i++) {
    char *comma = strchr(tmp, ',');
    if (comma) {
      *comma = '\0';
    }

    fprintf(stderr, "TEST %30s... ", tmp);
    data->test_fns[i]();
    if (run_state.failed) {
      ret |= 1;
      fprintf(stderr, "NAY\n");
      fprintf(stderr, "%s\n", run_state.msg);
      run_state.failed = false;
    } else {
      fprintf(stderr, "YAY\n");
    }

    if (!comma)
      break;

    tmp = comma + 2;
  }

  return ret;
}

void test_fail(const char *fmt, ...)
{
  run_state.failed = true;

  va_list va;
  va_start(va, fmt);
  vsnprintf(run_state.msg, sizeof(run_state.msg), fmt, va);
  va_end(va);
}

enum {
  PIPE_READ_END = 0,
  PIPE_WRITE_END = 1,
};

void fd_capture_open(fd_capture_t *cap, int fd_to_cap)
{
  cap->pipe[0] = cap->pipe[1] = -1;

  int ret = pipe(cap->pipe);
  RASSERT(!ret, "pipe() -> %d: %m", ret);

  ret = dup2(cap->pipe[PIPE_WRITE_END], fd_to_cap);
  RASSERT(ret >= 0, "dup2(%d, %d) -> %d: %m",
      cap->pipe[PIPE_WRITE_END],
      fd_to_cap,
      ret);

  int flags = fcntl(cap->pipe[PIPE_READ_END], F_GETFL);
  ret = fcntl(cap->pipe[PIPE_READ_END],
      F_SETFL, flags | O_NONBLOCK);
  RASSERT(!ret, "F_SETFL: %d: %m", ret);
}

error_code_t fd_capture_read(fd_capture_t *cap,
    void *user_buf, size_t buflen, size_t *read_out)
{
  uint8_t *buf = user_buf;
  ssize_t r;

  size_t bytes_read = 0, r_zu;
  while (bytes_read < buflen) {
    const size_t to_read = buflen - bytes_read;
    r = read(cap->pipe[PIPE_READ_END],
        buf + bytes_read, to_read);
    if (r < 0) {
      if (errno == EAGAIN)
        break;

      DEBUG(1, "pipe read -> %zd: %m", r);
      *read_out = 0;
      return ERROR_FAILURE;
    }
    r_zu = r;
    RASSERT(r_zu && r_zu <= to_read,
        "%zu/%zu", r_zu, to_read);
    bytes_read += r_zu;
  }

  *read_out = bytes_read;
  return bytes_read ? ERROR_SUCCESS : ERROR_TRY_AGAIN;
}
