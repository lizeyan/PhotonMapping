#ifndef PHOTONMAP_H
#define PHOTONMAP_H
#include "ray.h"
#include "color.h"
#include "object.h"
#include "photonbox.h"
#include <vector>
#include <memory>
class PhotonMap
{
public:
    PhotonMap();
    void store (const Photon& photon)   { _photons.push_back (std::move (std::unique_ptr<Photon> (new Photon(photon))));}
    Color search (const Vec3& point) const;
    void build ();
protected:
    PhotonBox* createKdTree (std::vector<PhotonBox>::iterator begin, std::vector<PhotonBox>::iterator end, int depth);
    void search (const Sphere& s, std::vector<Photon*>& res) const;
private:
    std::vector<std::unique_ptr<Photon> > _photons;
    std::unique_ptr<PhotonBox> _root;
};

#endif // PHOTONMAP_H
