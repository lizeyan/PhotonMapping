#include "photonmap.h"
#include <utility>
PhotonMap::PhotonMap()
{

}

Color PhotonMap::search (const Vec3 &point) const
{
    double l = 0, r = Bound, mi = (r + l) / 2;
    std::vector<Photon*> res;
    while (true)
    {
        res.clear ();
        search (Sphere (point, mi, nullptr, nullptr, false), _root.get (), res);
        if (res.size () == K && r - l <= errorLimit)
            break;
        if (res.size () > K)
            r = mi;
        else
            l = mi;
        mi = (r + l) / 2;
    }
    Color resColor;
    for (const auto& c: res)
        resColor += c->color;
    double scale = 1.0 / (static_cast<double>(res.size ()) * PI * mi * mi);
    return resColor * scale;
}

void PhotonMap::search (const Sphere &s, PhotonBox* v, std::vector<Photon*> &res) const
{
    if (v->isLeaf ())
    {
        if (v->intersect (s))
            res.push_back (v->photon ());
        return;
    }
    if (v->lc ()->contained (s))
        addBox (v->lc (), res);
    else if (v->lc ()->intersect (s))
        search (s, v->lc (), res);
    if (v->rc ()->contained (s))
        addBox (v->rc (), res);
    else if (v->rc ()->intersect (s))
        addBox (v->rc (), res);
}

void PhotonMap::addBox (PhotonBox *v, std::vector<Photon *> &res)
{
    if (v->isLeaf ())
        res.push_back (v->photon ());
    else
    {
        addBox (v->lc (), res);
        addBox (v->rc (), res);
    }
}

void PhotonMap::build ()
{
    _root.reset (createKdTree (begin (_photons), end (_photons), 0));
}

PhotonBox* PhotonMap::createKdTree (std::vector<std::unique_ptr<Photon>>::iterator begin, std::vector<std::unique_ptr<Photon>>::iterator end, int depth)
{
    static auto xcmp = [&] (const std::unique_ptr<Photon>& a, const std::unique_ptr<Photon>& b) -> bool
    {
        return a->point.arg (0) < b->point.arg (0);
    };
    static auto ycmp = [&] (const std::unique_ptr<Photon>& a, const std::unique_ptr<Photon>& b) -> bool
    {
        return a->point.arg (1) < b->point.arg (1);
    };
    static auto zcmp = [&] (const std::unique_ptr<Photon>& a, const std::unique_ptr<Photon>& b) -> bool
    {
        return a->point.arg (2) < b->point.arg (2);
    };
    int length = std::distance (begin, end);
    if (length == 1)
        return new PhotonBox (begin->get ());
    double minX = Bound, maxX = -Bound, minY = Bound, maxY = -Bound, minZ = Bound, maxZ = -Bound;
    for (auto c = begin; c != end; ++c)
    {
        if ((*c)->point.arg (0) < minX)
            minX = (*c)->point.arg (0);
        else if ((*c)->point.arg (0) > maxX)
            maxX = (*c)->point.arg (0);
        if ((*c)->point.arg (1) < minY)
            minY = (*c)->point.arg (1);
        else if ((*c)->point.arg (1) > maxY)
            maxY = (*c)->point.arg (1);
        if ((*c)->point.arg (2) < minZ)
            minZ = (*c)->point.arg (2);
        else if ((*c)->point.arg (2) > maxZ)
            maxZ = (*c)->point.arg (2);
    }
    int d = depth % 3;
    if (d == 0)
        std::nth_element (begin, begin + (length >> 1), end, xcmp);
    else if (d == 1)
        std::nth_element (begin, begin + (length >> 1), end, ycmp);
    else
        std::nth_element (begin, begin + (length >> 1), end, zcmp);
    return new PhotonBox (Vec3(std::array<double, 3>{{minX - EPS, minY - EPS, minZ - EPS}}), Vec3(std::array<double, 3>{{maxX + EPS, maxY + EPS, maxZ + EPS}} ), createKdTree (begin, begin + (length >> 1), depth + 1), createKdTree (begin + (length >> 1), end, depth + 1));
}


