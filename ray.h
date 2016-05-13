#ifndef RAY_H
#define RAY_H
#include "vector.h"
#include <utility>
typedef Vector<3> Vec3;
typedef std::pair<Vector<3>, Vector<3> > Ray;
double distance (const Vec3& a, const Ray& b);
double distance (const Ray& b, const Vec3 &a);
const Vec3 unitX(std::array<double, 3>{{1, 0, 0}});
const Vec3 unitY(std::array<double, 3>{{0, 1, 0}});
const Vec3 unitZ(std::array<double, 3>{{0, 0, 1}});
#endif // RAY_H
