#ifndef LIGHT_H
#define LIGHT_H
#include "ray.h"
#include "object.h"
#include "primitive.h"
#include "color.h"
#include <iostream>
typedef Vector<3> Vec3;
class Light: public Primitive
{
public:
    static Light* produce (const std::string& content, Condutor* condutor = nullptr);
    Light(const Color& color = Color (), Condutor* condutor = nullptr);
    virtual Ray emitPhoton () = 0;//发射一个光子，返回发射光子的光线。
    virtual void display (std::ostream& os) const;
    virtual Collide collide (const Ray& ray) const = 0;//检测光线和光源的碰撞，返回碰撞信息
    virtual Vec3 point () const = 0;//返回光源上的一个点
    virtual Color illuminate (const Vec3& point, const Vec3& normal) = 0;//返回光源照亮这个点的颜色。即光源对这个点的直接光照。
    virtual bool block (Object* ob, const Vec3&, Condutor* condutor) const = 0;
    inline Color color () {return _color;}//返回光源的颜色。
    inline void setColor (const Color& color) {_color = color;}
private:
    Color _color;
};
class PointLight: public Light
{
public:
    PointLight (std::stringstream& content, Condutor* condutor = nullptr);
    PointLight (const Vec3& center, const Color& color, Condutor* condutor = nullptr);
    Ray emitPhoton ();
    Collide collide (const Ray &ray) const;
    inline Vec3 point () const {return _center;}
    Color illuminate (const Vec3 &point, const Vec3 &normal);
    bool block (Object* ob, const Vec3&, Condutor* condutor) const;
    void display (std::ostream& os) const;
protected:
    void analyseContent (std::stringstream& content);
    void init ();
    bool check ();
private:
    Vec3 _center;
};


#endif // LIGHT_H
