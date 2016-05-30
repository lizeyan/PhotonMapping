#include "triangleBox.h"

TriangleBox::TriangleBox (const Vec3& lb, const Vec3 & rt, TriangleBox* lc, TriangleBox* rc, Object* object):Cobic((lb + rt) * 0.5, unitX, unitY, unitZ, rt.arg(0) - lb.arg (0), rt.arg (1) - lb.arg (1), rt.arg (2) - lb.arg (2)), _lc(lc), _rc(rc), _object (object), _leaf (false)
{

}

TriangleBox::TriangleBox (Cobic *c, Object* object):Cobic (*c), _object (object), _leaf (true)
{
}
