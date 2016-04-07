#ifndef LIGHT_H
#define LIGHT_H
#include "ray.h"
#include "primitive.h"
#include "color.h"
typedef Vector<3> Vec3;
class Light: public Primitive
{
public:
    static Light* produce (const std::string& content);
    Light();
    virtual Ray emitPhoton () = 0;//发射一个光子，返回发射光子的光线。
    virtual Collide collide (const Ray& ray) const = 0;//检测光线和光源的碰撞，返回碰撞信息
    virtual Vec3 point () const = 0;//返回光源上的一个点
    virtual Color illuminate (const Vec3& point, const Vec3& normal);//返回光源照亮这个点的颜色。即光源对这个点的直接光照。
    Color color () {return _color;}//返回光源的颜色；w：w。
private:
    Color _color;
};
class PointLight: public Light
{
public:
    PointLight (const Vec3& center);
    Ray emitPhoton ();
    Collide collide (const Ray &ray);
    inline Vec3 point () {return _center;}
private:
    Vec3 _center;
};


#endif // LIGHT_H
