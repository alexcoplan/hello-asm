#pragma once

#include <stdbool.h>

// On success, returns true and sets result.
// On failure (overflow, invalid input), returns false.
bool ajc_atoi_c(const char *str, int *result);

unsigned ajc_add_vargs_c(unsigned n, ...);
