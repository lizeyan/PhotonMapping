#ifndef KDNODE_H
#define KDNODE_H

#include "vector.h"
#include "object.h"
#include <memory>
class TriangleBox: public Cobic
{
public:
    TriangleBox (const Vec3& lb, const Vec3& rt, TriangleBox* lc, TriangleBox* rc, Object* object = nullptr);
    TriangleBox (Cobic* c, Object* object = nullptr);
    bool isLeaf () const {return _leaf;}
    Object* object () {return _object;}
    TriangleBox* lc () {return _lc.get ();}
    TriangleBox* rc () {return _rc.get ();}
private:
    std::unique_ptr<TriangleBox> _lc, _rc;
    Object* _object;
    bool _leaf;
};

#endif // KDNODE_H
