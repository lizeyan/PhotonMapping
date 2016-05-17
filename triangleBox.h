#ifndef KDNODE_H
#define KDNODE_H

#include "vector.h"
#include "object.h"
#include <memory>
class KdNode: public Cobic
{
public:
    KdNode (const Vec3& lb, const Vec3& rt, KdNode* lc, KdNode* rc, Object* object = nullptr);
    KdNode (Cobic* c, Object* object = nullptr);
    bool isLeaf () const {return _leaf;}
    Object* object () {return _object;}
    KdNode* lc () {return _lc.get ();}
    KdNode* rc () {return _rc.get ();}
private:
    std::unique_ptr<KdNode> _lc, _rc;
    Object* _object;
    bool _leaf;
};

#endif // KDNODE_H
