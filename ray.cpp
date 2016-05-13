#include "ray.h"
#include <cmath>
double distance (const Vec3& a, const Ray& b)
{
    if (fabs(model (b.second)) < EPS)
        throw std::logic_error ("unexcept direction of a ray");
    return model (cross(b.first - a, b.second)) / model (b.second);
}
double distance (const Ray& b, const Vec3 &a)
{
    return distance (a, b);
}
