#ifndef UTIL_H
#define UTIL_H

#include <cstdlib>
#include <cmath>

inline float rand_color()
{
  return (float) rand() / (float) RAND_MAX;
}

bool float_eq(float a, float b)
{
  if (fabs(a - b) < 0.01) return true;
  return false;
}

#endif

