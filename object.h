#ifndef OBJECT_H
#define OBJECT_H
#include "material.h"
#include "ray.h"
#include "primitive.h"
#include <string>
#include <memory>
typedef Vector<3> Vec3;
class Object: public Primitive
{
public:
    static Object* produce (const std::string& content);
    Object();
    virtual Collide collide (const Ray& ray) const = 0;
    Material* material () {return _material.get ();}
private:
    std::unique_ptr<Material> _material;
};
class Sphere: public Object
{
public:
    Sphere (const Vec3& center, float radius);
    Vec3 center () const {return _center;}
    float radius () const {return _radius;}
    Collide collide (const Ray &ray) const;
private:
    Vec3 _center;
    float _radius;
};

class Plane: public Object
{
public:
    Plane (const Vec3& center, const Vec3& normal);
    Vec3 center () const {return _center;}
    Vec3 normal () const {return _normal;}
    Collide collide (const Ray &ray) const;
private:
    Vec3 _center, _normal;
};

#endif // OBJECT_H
