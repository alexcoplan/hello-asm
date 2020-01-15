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

  return 0;
}
