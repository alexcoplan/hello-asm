#include "ajc_c.h"

#include <limits.h>
#include <stdarg.h>

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

unsigned ajc_add_vargs_c(unsigned n, ...)
{
  va_list ap;
  unsigned tmp, total;

  total = 0;

  va_start(ap, n);
  for (unsigned i = 0; i < n; i++) {
    tmp = va_arg(ap, unsigned);
    total += tmp;
  }
  va_end(ap);

  return total;
}
