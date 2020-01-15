#include <stdio.h>

// implemented in asm!
unsigned add(unsigned, unsigned);

int main(void)
{
  const unsigned ret = add(2, 3);
  printf("Result is %u\n", ret);
  return 0;
}
