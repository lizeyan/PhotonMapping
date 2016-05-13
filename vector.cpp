#include "vector.h"
#include <cmath>


Vector<3> vertical (const Vector<3> &v, const Vector<3> &p)
{
    if (fabs(v.arg (2)) > EPS)
    {
        double z = (v.arg (0) * (p.arg (0) - 1) + v.arg (1) * (p.arg (1) - 1) + v.arg (2) * p.arg (2)) / v.arg (2);
        return Vector<3>(std::array<double, 3>{{1, 1, z}});
    }
    else if (fabs(v.arg (1)) > EPS)
    {
        double b = (v.arg (0) * (p.arg (0) - 1) + v.arg (2) * (p.arg (2) - 1) + v.arg (1) * p.arg (1)) / v.arg (1);
        return Vector<3>(std::array<double, 3>{{1, b, 1}});
    }
    else if (fabs(v.arg (1)) > EPS)
    {
        double a = (v.arg (2) * (p.arg (2) - 1) + v.arg (1) * (p.arg (1) - 1) + v.arg (0) * p.arg (0)) / v.arg (0);
        return Vector<3>(std::array<double, 3>{{a, 1, 1}});
    }
    else
        throw std::logic_error ("trying to find a vector vertical to a zero vector");
}


