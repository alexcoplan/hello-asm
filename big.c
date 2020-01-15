#include "ajc_lib.h"
#include <stdio.h>

int main(void)
{
  const unsigned result = ajc_add(2, 3);
  printf("2 + 3 is %u\n", result);
  return 0;
}
