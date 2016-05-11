#ifndef KDTREE_H
#define KDTREE_H
#include "ray.h"
#include "kdnode.h"
#include "object.h"
#include "vector.h"
#include <memory>
#include <vector>
class Condutor;
class KdTree
{
public:
    KdTree(Condutor* condutor);
    std::vector<std::pair<Object*, Collide> > kdSearch (const Ray& ray) const;
protected:
    void buildTree ();
    KdNode* createKdTree (std::vector<KdNode*>::iterator begin, std::vector<KdNode*>::iterator end, int depth);
    void search (const Ray& ray, KdNode* v, std::vector<std::pair<Object*, Collide> >& res) const;
private:
    Condutor* _condutor;
    std::unique_ptr<KdNode> _root;
};

#endif // KDTREE_H
