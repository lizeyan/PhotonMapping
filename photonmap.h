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
    PhotonMap(Condutor* condutor);
    void store (const Photon& photon);
    std::vector<std::pair<Photon*, double> > search (const Vec3& point) const;
    void build ();
    void scale ();
    size_t size() {return _photons.size ();}
protected:
    PhotonBox* createKdTree (std::vector<std::unique_ptr<Photon>>::iterator begin, std::vector<std::unique_ptr<Photon>>::iterator end, int depth, const Vec3& lb, const Vec3& rt);
    void search (const Sphere& s, PhotonBox* v,  std::vector<Photon*>& res) const;
    bool search (const Vec3& p, PhotonBox* v, std::vector<PhotonBox*>& path) const;
    static void addBox (PhotonBox* v, std::vector<Photon*>& res);
private:
    std::vector<std::unique_ptr<Photon> > _photons;
    std::unique_ptr<PhotonBox> _root;
    Condutor* _condutor;
};

#endif // PHOTONMAP_H
