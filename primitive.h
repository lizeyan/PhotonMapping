#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include "ray.h"
#include "vector.h"
#include <iostream>
class Condutor;
typedef Vector<3> Vec3;
struct Collide
{
    bool collide;
    float distance;
    Vec3 point, normal;
};

class Primitive
{
public:
    Primitive(Condutor* condutor = nullptr);
    virtual Collide collide (const Ray& ray) const = 0;
    virtual void display (std::ostream& os ) const;
    Condutor* condutor () {return _condutor;}
    void setCondutor (Condutor* c) {_condutor = c;}
private:
    Condutor* _condutor;
};

std::ostream& operator<< (std::ostream& os, const Primitive& primitive);
#endif // PRIMITIVE_H
