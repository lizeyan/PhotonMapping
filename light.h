#ifndef LIGHT_H
#define LIGHT_H
#include "ray.h"
#include "object.h"
#include "primitive.h"
#include "color.h"
#include "photonmap.h"
#include <iostream>
typedef Vector<3> Vec3;
class Object;
class Light: public Primitive
{
public:
    static Light* produce (const std::string& content, Condutor* condutor = nullptr);
    Light(const Color& color = Color (), Condutor* condutor = nullptr);
    virtual Photon emitPhoton () = 0;//发射一个光子，返回发射光子的光线。
    virtual Photon emitPhoton (Object* object) = 0;//发射一个光子，返回发射光子的光线。
    virtual void display (std::ostream& os) const;
    virtual Collide collide (const Ray& ray) const = 0;//检测光线和光源的碰撞，返回碰撞信息
    virtual Vec3 point () const = 0;//返回光源上的一个点
    virtual Color illuminate (const Vec3& point, const Vec3& normal) = 0;//返回光源照亮这个点的颜色。即光源对这个点的直接光照。要求normal必须是单位向量
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
    Photon emitPhoton ();
    Photon emitPhoton (Object* object);//发射一个光子，返回发射光子的光线。
    Collide collide (const Ray &ray) const;
    inline Vec3 point () const {return _center;}
    Color illuminate (const Vec3 &point, const Vec3 &normal);
    void display (std::ostream& os) const;
protected:
    void analyseContent (std::stringstream& content);
    void init ();
    bool check ();
private:
    Vec3 _center;
};

class RectLight: public Light
{
public:
    RectLight (std::stringstream& content, Condutor* condutor = nullptr);
    RectLight (const Vec3& center, const Vec3& normal, const Vec3& dx, const Vec3& dy, double width, double height, const Color& color, Condutor* condutor = nullptr);
    Photon emitPhoton ();
    Photon emitPhoton (Object* object);//发射一个光子，返回发射光子的光线。
    Collide collide (const Ray &ray) const;
    inline Vec3 point () const {return _center;}
    Color illuminate (const Vec3 &point, const Vec3 &normal);
    void display (std::ostream& os) const;
protected:
    void analyseContent (std::stringstream& content);
    void init ();
    bool check ();
    void preHandle ();
private:
    Vec3 _center, _normal, _dx, _dy;
    double _width, _height;
    double _widthHalf, _heightHalf;
    size_t _quality;
};

class CircleLight: public Light
{
public:
    CircleLight (std::stringstream& content, Condutor* condutor = nullptr);
    CircleLight (const Vec3& center, const Vec3& normal, double radius, const Color& color, Condutor* condutor = nullptr);
    Photon emitPhoton ();
    Photon emitPhoton (Object* object);//发射一个光子，返回发射光子的光线。
    Collide collide (const Ray &ray) const;
    inline Vec3 point () const {return _center;}
    Color illuminate (const Vec3 &point, const Vec3 &normal);
    void display (std::ostream& os) const;
protected:
    void analyseContent (std::stringstream& content);
    void init ();
    bool check ();
    void preHandle ();
private:
    Vec3 _center, _normal;
    double _radius;
    Vec3 _dx, _dy;
    size_t _quality;
};

#endif // LIGHT_H
