#ifndef MATHS_H
#define MATHS_H

namespace Maths {

float max(float a, float b);
float min(float a, float b);
float lerp(float start, float end, float t);
double integrate(double (*function)(double), const unsigned int steps, const double low, const double high);

} // namespace Maths

#endif