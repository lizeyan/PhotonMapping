#ifndef PHOTONMAP_H
#define PHOTONMAP_H
#include "ray.h"
#include "color.h"
#include "object.h"
#include "photonbox.h"
#include <vector>
#include <memory>
const double errorLimit = 1e-3;
class Condutor;
class PhotonMap
{
public:
    typedef std::vector<std::unique_ptr<Photon> > PhotonVec;
    PhotonMap(Condutor* condutor);
    void store (const Photon& photon);
    std::vector<std::pair<Photon*, double> > search (const Vec3& point) const;
    void build ();
    size_t size() {return _photons.size ();}
protected:
    PhotonBox* createKdTree (PhotonVec::iterator begin, PhotonVec::iterator end, int depth = 0);
private:
    std::vector<std::unique_ptr<Photon> > _photons;
    std::unique_ptr<PhotonBox> _root;
    Condutor* _condutor;
};

#endif // PHOTONMAP_H
