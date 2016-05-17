#include "photonmap.h"
#include <utility>
PhotonMap::PhotonMap()
{

}

Color PhotonMap::search (const Vec3 &point) const
{

}

void PhotonMap::build ()
{
    _root.reset (createKdTree (begin (_photons), end (_photons), 0));
}

PhotonBox* PhotonMap::createKdTree (std::vector::iterator begin, std::vector::iterator end, int depth)
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
        if ((*c)->xl() < minX)
            minX = (*c)->xl();
        if ((*c)->xh() > maxX)
            maxX = (*c)->xh ();
        if ((*c)->yl() < minY)
            minY = (*c)->yl();
        if ((*c)->yh() > maxY)
            maxY = (*c)->yh ();
        if ((*c)->zl() < minZ)
            minZ = (*c)->zl();
        if ((*c)->zh() > maxZ)
            maxZ = (*c)->zh ();
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


