#include "kdtree.h"
#include "object.h"
#include "condutor.h"
#include <algorithm>
#include <vector>
#include <utility>
#ifdef DEBUG
extern std::ofstream Log;
#endif
KdTree::KdTree(Condutor* condutor): _condutor (condutor)
{
    buildTree ();
}

std::vector<std::pair<Object*, Collide> > KdTree::kdSearch (const Ray &ray) const
{
    std::vector<std::pair<Object*, Collide> >res;
    this->search (ray, _root.get (), res);
    return std::move (res);
}

void KdTree::search (const Ray &ray, KdNode *v, std::vector<std::pair<Object*, Collide> >& res) const
{
    if (v->isLeaf ())
    {
        Collide tmp = v->object()->collide (ray);
        if (tmp.collide)
            res.push_back (std::make_pair (v->object(), std::move(tmp)));
        return;
    }
    auto tmp1 = v->lc ()->collide (ray);
    auto tmp2 = v->rc ()->collide (ray);
    if (tmp1.collide)
        search (ray, v->lc (), res);
    if (tmp2.collide)
        search (ray, v->rc (), res);
}

void KdTree::buildTree ()
{
    std::vector<KdNode*> leafs;
    for (const auto& o: _condutor->objects ())
    {
#ifdef DEBUG
        Log << "leaf:" << *o << std::endl;
#endif
        leafs.push_back (new KdNode (o->boudingBox(), o.get ()));
    }
    _root.reset (createKdTree (std::begin(leafs), std::end(leafs), 0));
}

KdNode* KdTree::createKdTree (std::vector<KdNode*>::iterator begin, std::vector<KdNode*>::iterator end, int depth)
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
    float minX = Bound, maxX = -Bound, minY = Bound, maxY = -Bound, minZ = Bound, maxZ = -Bound;
    int d = depth % 3;
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
    if (d == 0)
        std::nth_element (begin, begin + (length >> 1), end, xcmp);
    else if (d == 1)
        std::nth_element (begin, begin + (length >> 1), end, ycmp);
    else
        std::nth_element (begin, begin + (length >> 1), end, zcmp);
    KdNode* lc = createKdTree (begin, begin + (length >> 1), depth + 1);
    KdNode* rc = createKdTree (begin + (length >> 1), end, depth + 1);
    return new KdNode (Vec3(std::array<float, 3>{{minX, minY, minZ}}), Vec3(std::array<float, 3>{{maxX, maxY, maxZ}} ), lc, rc);
}
