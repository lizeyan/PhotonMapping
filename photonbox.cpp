#include "photonbox.h"

PhotonBox::PhotonBox(const Vec3& lb, const Vec3 & rt, PhotonBox* lc, PhotonBox* rc, Photon* photon): _photon (nullptr), _box ((lb + rt) * 0.5, unitX, unitY, unitZ, rt.arg(0) - lb.arg (0), rt.arg (1) - lb.arg (1), rt.arg (2) - lb.arg (2)), _lc (lc), _rc (rc)
{

}

PhotonBox::PhotonBox (Photon *photon): _photon (photon)
{

}

bool PhotonBox::intersect (const Sphere& s) const
{

}
