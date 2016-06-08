#ifndef PHOTONBOX_H
#define PHOTONBOX_H
#include <memory>
#include "color.h"
#include "object.h"
struct Photon
{
    Vec3 point, dir;
    Color color;
};

class PhotonBox
{
public:
    PhotonBox (PhotonBox* lc, PhotonBox* rc, Photon* photon, int dimension, double split);
    Photon* photon () const {return _photon;}
    PhotonBox* lc () const {return _lc.get ();}
    PhotonBox* rc () const {return _rc.get ();}
    int dimension () const {return _dimension;}
    double split () const {return _split;}
private:
    std::unique_ptr<PhotonBox> _lc, _rc;
    Photon* _photon;
    int _dimension;
    double _split;
};

#endif // PHOTONBOX_H
