#include "photonbox.h"

PhotonBox::PhotonBox(const Vec3& lb, const Vec3 & rt, PhotonBox* lc, PhotonBox* rc): _photon (nullptr), _box ((lb + rt) * 0.5, unitX, unitY, unitZ, rt.arg(0) - lb.arg (0), rt.arg (1) - lb.arg (1), rt.arg (2) - lb.arg (2)), _lc (lc), _rc (rc)
{

}

PhotonBox::PhotonBox (Photon *photon): _photon (photon), _box (photon->point, unitX, unitY, unitZ, EPS, EPS, EPS), _lc (nullptr), _rc (nullptr)
{

}

bool PhotonBox::intersect (const Sphere& s) const
{
    double x = s.center ().arg (0), y = s.center ().arg (1), z = s.center ().arg (2);
    if (x < _box.xl ())
        x = _box.xl ();
    else if (x > _box.xh ())
        x = _box.xh ();
    if (y < _box.yl ())
        y = _box.yl ();
    else if (y > _box.yh ())
        y = _box.yh ();
    if (z < _box.zl ())
        z = _box.zl ();
    else if (z > _box.zh ())
        z = _box.zh ();
    double dx = x - s.center ().arg (0), dy = y - s.center ().arg (1), dz = z - s.center ().arg (2);
    if (s.radius2 () >= dx * dx + dy *dy + dz * dz)
        return true;
    else
        return false;
}

bool PhotonBox::contained (const Sphere &s) const
{
    if (isLeaf ())
    {
        double dx = _photon->point.arg(0) - s.center ().arg (0), dy = _photon->point.arg (1) - s.center ().arg (1), dz = _photon->point.arg (2) - s.center ().arg (2);
        return s.radius2 () >= dx * dx + dy * dy + dz * dz;
    }
    double dx = _box.xl () - s.center ().arg (0), dy = _box.yl () - s.center ().arg (1), dz = _box.zl () - s.center ().arg (2);
    if (s.radius2 () < dx * dx + dy * dy + dz * dz)
        return false;
    dx = _box.xh () - s.center ().arg (0), dy = _box.yh () - s.center ().arg (1), dz = _box.zh () - s.center ().arg (2);
    if (s.radius2 () < dx * dx + dy * dy + dz * dz)
        return false;
    else
        return true;
}
