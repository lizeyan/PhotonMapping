#ifndef LIGHT_H
#define LIGHT_H
#include "ray.h"
#include "color.h"
typedef Vector<3> Vec3;
class Light
{
public:
    static Light* produce (const std::string& content);
    Light();
    virtual Ray emitPhoton () = 0;
    virtual bool collide (const Ray& ray) const = 0;
    virtual Vec3 center () const = 0;
};
class PointLight: public Light
{
public:
    PointLight (const Vec3& center);
    Ray emitPhoton ();
    bool collide (const Ray &ray);
    inline Vec3 center () {return _center;}
private:
    Vec3 _center;
};


#endif // LIGHT_H
