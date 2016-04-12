#include "vector.h"
Vector<3> cross (const Vector<3>& a, const Vector<3>& b)
{
    return Vector<3>(std::array<float, 3>{{a.arg(1) * b.arg(2)-a.arg(2) * b.arg(1), a.arg(2) * b.arg(0) - a.arg(0) * b.arg(2), a.arg(0) * b.arg(1) - a.arg(1) * b.arg(0)}});
}
