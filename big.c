#include "ajc_lib.h"
#include <stdio.h>

int main(void)
{
  const unsigned result = ajc_add(2, 3);
  printf("2 + 3 is %u\n", result);

  const char *str = "hello";
  size_t len = ajc_strlen(str);
  printf("%s is %zu bytes long\n", str, len);

  str = "wibble";
  len = ajc_strlen(str);
  printf("%s is %zu bytes long\n", str, len);

  const char *seven = "7";
  int ret;
  unsigned ok = ajc_atoi(seven, &ret);
  printf("%s -> (ok=%u, ret=%d)\n", seven, ok, ret);
  return 0;
}
