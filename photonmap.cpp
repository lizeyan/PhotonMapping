#include "photonmap.h"
#include <utility>
#include <stack>
#include <queue>
#include <iterator>
PhotonMap::PhotonMap()
{

}

std::pair<std::vector<Photon*>, double> PhotonMap::search (const Vec3 &point) const
{
//    std::cout << "point:" << point << std::endl;
    std::vector<PhotonBox*> path;
    search (point, _root.get (), path);
    auto it = begin (path);
    auto photonLess = [] (const std::pair<Photon*, double>& a, const std::pair<Photon*, double>& b) -> bool
    {
        return a.second < b.second;
    };
    std::priority_queue<std::pair<Photon*, double>, std::vector<std::pair<Photon*, double> >, decltype(photonLess) > photons (photonLess);
    auto insertInto = [this, &photons, &point] (Photon* p)
    {
        double dis = distance (p->point, point);
        if (photons.size () < K)
            photons.push (std::make_pair (p, dis));
        else if (dis < photons.top ().second)
        {
            photons.pop ();
            photons.push (std::make_pair (p, dis));
        }
    };
    insertInto ((*it)->lc ()->photon ());
    insertInto ((*it)->rc ()->photon ());
    ++it;
    while (it != end (path))
    {
        PhotonBox* last = *std::prev (it);
        Sphere s (point, photons.top ().second, nullptr, nullptr, false);
        if ((*it)->lc () != last && (*it)->lc ()-> intersect (s))
        {
            std::vector<Photon*> ps;
            search (s,  (*it)->lc (), ps);
            for (const auto& p: ps)
                insertInto (p);
        }
        else if ((*it)->rc () != last && (*it)->rc ()-> intersect (s))
        {

            std::vector<Photon*> ps;
            search (s, (*it)->rc (), ps);
            for (const auto& p: ps)
                insertInto (p);
        }
        ++it;
    }
    std::vector<Photon*> res;
    for (unsigned i = 0; i < K; ++i)
    {
        res.push_back (photons.top ().first);
        photons.pop ();
    }
    return std::make_pair (std::move (res), distance (res.front ()->point, point));
}

bool PhotonMap::search (const Vec3 &p, PhotonBox *v, std::vector<PhotonBox *> &path) const
{
    if (v->lc ()->isLeaf ())//子节点就是叶节点
    {
        if (v->contain (p))
        {
            path.push_back (v);
            return true;
        }
        else
            return false;
    }
    if (search (p, v->lc (), path) || search (p, v->rc (), path))
    {
        path.push_back (v);
        return true;
    }
    else
        return false;
}

void PhotonMap::search (const Sphere &s, PhotonBox* v, std::vector<Photon*> &res) const
{
    if (v->isLeaf ())
    {
        if (v->contained (s))
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
        search (s, v->rc (), res);
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


