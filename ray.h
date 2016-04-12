#ifndef RAY_H
#define RAY_H
#include "vector.h"
#include <utility>
typedef Vector<3> Vec3;
typedef std::pair<Vector<3>, Vector<3> > Ray;
float distance (const Vec3& a, const Ray& b);
float distance (const Ray& b, const Vec3 &a);

#endif // RAY_H
