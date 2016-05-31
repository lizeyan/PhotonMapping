#ifndef PHOTONMAP_H
#define PHOTONMAP_H
#include "ray.h"
#include "color.h"
#include "object.h"
#include "photonbox.h"
#include <vector>
#include <memory>
const unsigned int K = 16;
const double errorLimit = 1e-3;
class PhotonMap
{
public:
    PhotonMap();
    void store (const Photon& photon)   { _photons.push_back (std::move (std::unique_ptr<Photon> (new Photon(photon))));}
    std::pair<std::vector<Photon*>, double> search (const Vec3& point) const;
    void build ();
    size_t size() {return _photons.size ();}
protected:
    PhotonBox* createKdTree (std::vector<std::unique_ptr<Photon>>::iterator begin, std::vector<std::unique_ptr<Photon>>::iterator end, int depth);
    void search (const Sphere& s, PhotonBox* v,  std::vector<Photon*>& res) const;
    bool search (const Vec3& p, PhotonBox* v, std::vector<PhotonBox*>& path) const;
    static void addBox (PhotonBox* v, std::vector<Photon*>& res);
private:
    std::vector<std::unique_ptr<Photon> > _photons;
    std::unique_ptr<PhotonBox> _root;
};

#endif // PHOTONMAP_H
