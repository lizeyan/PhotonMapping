#include "triangleTree.h"
#include "object.h"
#include "condutor.h"
#include <algorithm>
#include <vector>
#include <utility>
#ifdef DEBUG
extern std::ofstream Log;
#endif
TriangleTree::TriangleTree(Condutor* condutor): _condutor (condutor)
{
    buildTree ();
}

std::vector<std::pair<Object*, Collide> > TriangleTree::kdSearch (const Ray &ray) const
{
    Ray r(ray);
    r.second = standardize (ray.second);
    std::vector<std::pair<Object*, Collide> >res;
    this->search (r, _root.get (), res);
    return std::move (res);
}

void TriangleTree::search (const Ray &ray, TriangleBox *v, std::vector<std::pair<Object*, Collide> >& res) const
{
    if (v->isLeaf ())
    {
        Collide tmp = v->object()->collide (ray);
        if (tmp.collide)
            res.push_back (std::make_pair (v->object(), std::move(tmp)));
        return;
    }
    Collide tmp1 = v->lc ()->collide (ray);
    Collide tmp2 = v->rc ()->collide (ray);
    if (tmp1.collide)
        search (ray, v->lc (), res);
    if (tmp2.collide)
        search (ray, v->rc (), res);
}

void TriangleTree::buildTree ()
{
    std::vector<TriangleBox*> leafs;
    for (const auto& o: _condutor->objects ())
    {
#ifdef DEBUG
        Log << "leaf:" << *o << std::endl;
#endif
        leafs.push_back (new TriangleBox (o->boudingBox(), o.get ()));
    }
#ifdef DEBUG
    Log << "all leafs: " << leafs.size () << std::endl;
#endif
    _root.reset (createKdTree (std::begin(leafs), std::end(leafs), 0));
}

TriangleBox* TriangleTree::createKdTree (std::vector<TriangleBox*>::iterator begin, std::vector<TriangleBox*>::iterator end, int depth)
{
    static auto xcmp = [&] (const Cobic* a, const Cobic* b) -> bool
    {
        return a->xh () < b->xh ();
    };
    static auto ycmp = [&] (const Cobic* a, const Cobic* b) -> bool
    {
        return a->yh () < b->yh ();
    };
    static auto zcmp = [&] (const Cobic* a, const Cobic* b) -> bool
    {
        return a->zh () < b->zh ();
    };
    int length = std::distance (begin ,end);
    if (length == 1)
        return *begin;
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
    TriangleBox* lc = createKdTree (begin, begin + (length >> 1), depth + 1);
    TriangleBox* rc = createKdTree (begin + (length >> 1), end, depth + 1);
    return new TriangleBox (Vec3(std::array<double, 3>{{minX - EPS, minY - EPS, minZ - EPS}}), Vec3(std::array<double, 3>{{maxX + EPS, maxY + EPS, maxZ + EPS}} ), lc, rc);
}
