#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include "ray.h"
#include "vector.h"
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
    Primitive();
    virtual Collide collide (const Ray& ray) = 0;
};

#endif // PRIMITIVE_H
