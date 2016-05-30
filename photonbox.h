#ifndef PHOTONBOX_H
#define PHOTONBOX_H
#include <memory>
#include "color.h"
#include "object.h"
struct Photon
{
    Vec3 point;
    Color color;
};

class PhotonBox
{
public:
    PhotonBox (const Vec3& lb, const Vec3 & rt, PhotonBox* lc, PhotonBox* rc);
    PhotonBox (Photon* photon);
    bool isLeaf () const {return _photon != nullptr;}
    PhotonBox* lc () {return _lc.get ();}
    PhotonBox* rc () {return _rc.get ();}
    Photon* photon () {return _photon;}
    bool intersect (const Sphere& s) const;
    bool contained (const Sphere& s) const;
private:
    Photon* _photon;
    Cobic _box;
    std::unique_ptr<PhotonBox> _lc, _rc;
};

#endif // PHOTONBOX_H
