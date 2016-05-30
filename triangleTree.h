#ifndef KDTREE_H
#define KDTREE_H
#include "ray.h"
#include "triangleBox.h"
#include "object.h"
#include "vector.h"
#include <memory>
#include <vector>
class Condutor;
class TriangleTree
{
public:
    TriangleTree(Condutor* condutor);
    std::vector<std::pair<Object*, Collide> > kdSearch (const Ray& ray) const;
protected:
    void buildTree ();
    TriangleBox* createKdTree (std::vector<TriangleBox*>::iterator begin, std::vector<TriangleBox*>::iterator end, int depth);
    void search (const Ray& ray, TriangleBox* v, std::vector<std::pair<Object*, Collide> >& res) const;
private:
    Condutor* _condutor;
    std::unique_ptr<TriangleBox> _root;
};

#endif // KDTREE_H
