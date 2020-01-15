#include "ajc_c.h"

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

int LLVMFuzzerTestOneInput(const uint8_t *buf, size_t buflen)
{
  char *mem = malloc(buflen+1);
  memcpy(mem, buf, buflen);
  mem[buflen] = '\0';

  int sscanf_value;
  const int sscanf_ret = sscanf(mem, "%d", &sscanf_value);
  const bool sscanf_ok = (sscanf_ret == 1);

  int ajc_value;
  bool ajc_ok = ajc_atoi_c(mem, &ajc_value);

  if (sscanf_ok) {
    char normal[64];
    snprintf(normal, sizeof(normal), "%d", sscanf_value);
    if (strcmp(normal, mem)) {
      if (ajc_ok) {
        assert(!strcmp(mem, "-0"));
        assert(sscanf_value == 0);
        assert(ajc_value == 0);
      }
    } else {
      assert(ajc_ok);
      assert(ajc_value == sscanf_value);
    }
  } else {
    assert(!ajc_ok);
  }

  free(mem);
  return 0;
}
