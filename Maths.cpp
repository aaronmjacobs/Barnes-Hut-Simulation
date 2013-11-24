#include "Maths.h"

namespace Maths {
  
float max(float a, float b) {
  return a > b ? a : b;
}

float min(float a, float b) {
  return a < b ? a : b;
}

float lerp(float start, float end, float t) {
  return start + (end - start) * t;
}

double integrate(double (*function)(double), const unsigned int steps, const double low, const double high) {
  const float step = (high - low) / steps;
  const float halfStep = step / 2.0;

  double x = low;
  double sum = 0.0;
  
  while (x + halfStep < high) {
    sum += step * function(x + halfStep);
    x += step;
  }

  return sum;
}

} // namespace Maths