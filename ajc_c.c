#include "ajc_c.h"

#include <limits.h>

bool ajc_atoi_c(const char *str, int *result)
{
  const unsigned neg = (*str == '-');
  if (neg)
    str++;

  // "" and "-" are invalid.
  if (!str[0])
    return false;

  // Leading zero is only valid if it's the only digit.
  if (str[0] == '0' && str[1] != '\0')
    return false;

  int acc = 0;
  for (; *str; str++) {
    if (*str < '0' || *str > '9')
      return false;

    if (acc > INT_MAX / 10)
      return false; // Overflow

    acc *= 10;
    acc += (*str - '0');
  }

  *result = neg ? -acc : acc;
  return true;
}
