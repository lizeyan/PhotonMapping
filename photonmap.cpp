#include "photonmap.h"
#include "condutor.h"
#include <utility>
#include <stack>
#include <queue>
#include <iterator>
PhotonMap::PhotonMap(Condutor* condutor):_condutor (condutor)
{

}

std::vector<std::pair<Photon*, double> > PhotonMap::search (const Vec3 &point) const
{
//    std::cout << "point:" << point << std::endl;
    std::vector<PhotonBox*> path;
    search (point, _root.get (), path);
    if (path.size () == 0)
    {
        std::cout << "point:" << point << std::endl;
        throw std::logic_error ("path.size == 0");
    }
    auto it = begin (path);
    auto photonLess = [] (const std::pair<Photon*, double>& a, const std::pair<Photon*, double>& b) -> bool {return a.second < b.second;};
    std::priority_queue<std::pair<Photon*, double>, std::vector<std::pair<Photon*, double> >, decltype(photonLess) > photons (photonLess);
    auto insertInto = [this, &photons, &point] (Photon* p)
    {
        double dis = distance (p->point, point);
        if (photons.size () < _condutor->camera ()->K ())
            photons.push (std::make_pair (p, dis));
        else if (dis < photons.top ().second)
        {
            photons.pop ();
            photons.push (std::make_pair (p, dis));
        }
    };
    std::stack<PhotonBox*> s;
    s.push (*it);
    while (!s.empty ())
    {
        PhotonBox* top = s.top ();
        s.pop ();
        if (top->isLeaf ())
            insertInto (top->photon ());
        else
        {
            s.push (top->lc ());
            s.push (top->rc ());
        }
    }
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
    std::vector<std::pair<Photon*, double> > res;
    for (size_t i = 0; i < _condutor->camera ()->K (); ++i)
    {
        res.push_back (photons.top ());
        photons.pop ();
    }
    return std::move (res);
}

bool PhotonMap::search (const Vec3 &p, PhotonBox *v, std::vector<PhotonBox *> &path) const
{
    if (v->lc ()->isLeaf () && v->rc ()->isLeaf ())
    {
        if (v->contain (p))
        {
            path.push_back (v);
            return true;
        }
        else
            return false;
    }
    else if (v->lc ()->isLeaf ())
    {
        if (search (p, v->rc (), path) || v->contain (p))
        {
            path.push_back (v);
            return true;
        }
        else
            return false;
    }
    else if (v->rc ()->isLeaf ())
    {
        if (search (p, v->lc (), path) || v->contain (p))
        {
            path.push_back (v);
            return true;
        }
        else
            return false;
    }
    else
    {
        bool lc = search (p, v->lc (), path);
        bool rc = search (p, v->rc (), path);
        if (lc || rc)
        {
            path.push_back (v);
            return true;
        }
        else
            return false;
    }
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
    std::ofstream fout ("photons.txt");
    for (const auto& photon: _photons)
    {
        fout << photon->point.arg(0) << " " << photon->point.arg(1) << " " << photon->point.arg(2)  << std::endl;
    }
    fout.close ();
    _root.reset (createKdTree (begin (_photons), end (_photons), 0, _condutor->lb (), _condutor->rt ()));
}

void PhotonMap::store (const Photon &photon)
{
    Vec3 lb = _condutor->lb (), rt = _condutor->rt (), p = photon.point;
    if (p.arg (0) <= rt.arg (0) && p.arg (0) >= lb.arg (0) && p.arg (1) <= rt.arg (1) && p.arg (1) >= lb.arg (1) && p.arg (2) <= rt.arg (2) && p.arg (2) >= lb.arg (2))
        _photons.push_back (std::move (std::unique_ptr<Photon> (new Photon(photon))));
}

PhotonBox* PhotonMap::createKdTree (std::vector<std::unique_ptr<Photon>>::iterator begin, std::vector<std::unique_ptr<Photon>>::iterator end, int depth, const Vec3& lb, const Vec3& rt)
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
    int d = depth % 3;
    Vec3 lbTmp (lb), rtTmp (rt);
    if (d == 0)
    {
        std::nth_element (begin, begin + (length >> 1), end, xcmp);
        double x = (*(begin + (length >> 1)))->point.arg (0);
        lbTmp[0] = x;
        rtTmp[0] = x + EPS;
    }
    else if (d == 1)
    {
        std::nth_element (begin, begin + (length >> 1), end, ycmp);
        double y = (*(begin + (length >> 1)))->point.arg (1);
        lbTmp[1] = y;
        rtTmp[1] = y + EPS;
    }
    else
    {
        std::nth_element (begin, begin + (length >> 1), end, zcmp);
        double z = (*(begin + (length >> 1)))->point.arg (2);
        lbTmp[2] = z;
        rtTmp[2] = z + EPS;
    }
    return new PhotonBox (lb, rt, createKdTree (begin, begin + (length >> 1), depth + 1, lb, rtTmp), createKdTree (begin + (length >> 1), end, depth + 1, lbTmp, rt));
}


