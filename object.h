#ifndef OBJECT_H
#define OBJECT_H
#include "material.h"
#include "ray.h"
#include "primitive.h"
#include <string>
#include <memory>
#include <iostream>
typedef Vector<3> Vec3;
class Cobic;
class Object: public Primitive
{
public:
    static Object* produce (const std::string& content, Condutor* condutor = nullptr);
    virtual ~Object() {}
    Object(Material* = nullptr, Condutor* condutor = nullptr);
    virtual Collide collide (const Ray& ray) const = 0;//返回的碰撞点法向必须是单位向量.传入的应该是单位向量
    virtual void display (std::ostream& os) const;
    virtual Vec3 getRandomLink (const Vec3& source) const = 0;//应当返回单位向量
    Material* material () const {return _material;}
    virtual Color color (const Vec3& v = Vec3 ()) const = 0;
    inline void setMaterial (Material* m) {_material = m;}
    Cobic* boudingBox () const {return _boudingBox;}//应当每个object的这个函数至多被访问一次，在建立kd树的时候。要和坐标轴平行
protected:
    Material* _material;
    Cobic* _boudingBox;
};
class Sphere: public Object
{
public:
    Sphere (const Vec3& center, double radius, Material* material = nullptr, Condutor* condutor = nullptr, bool need = false);
    Sphere (std::stringstream& content, Condutor* condutor = nullptr);
    virtual ~Sphere ();
    Vec3 center () const {return _center;}
    double radius () const {return _radius;}
    double radius2 () const {return _r2;}
    Vec3 getRandomLink (const Vec3& source) const;//应当返回单位向量
    virtual Collide collide (const Ray &ray) const;
    virtual void display (std::ostream& os) const;
    virtual Color color (const Vec3 &v = Vec3 ()) const;
protected:
    void analyseContent (std::stringstream& content);
    bool check ();
    void init ();
    void preHandle ();
private:
    Vec3 _center;
    double _radius;

    double _r2;
    bool _needBoudingBox;
};

class Plane: public Object
{
public:
    Plane (const Vec3& center, const Vec3& normal, Material* material = nullptr, Condutor* condutor = nullptr, bool need = false);
    Plane (std::stringstream& content, Condutor* condutor = nullptr);
    virtual ~Plane ();
    Vec3 center () const {return _center;}
    Vec3 normal () const {return _normal;}
    Vec3 getRandomLink (const Vec3& source) const;//应当返回单位向量
    void display (std::ostream& os) const;
    Collide collide (const Ray &ray) const;
    double calc (const Vec3 & point) const;
    virtual Color color (const Vec3 &v = Vec3 ()) const;
protected:
    void analyseContent (std::stringstream& content);
    bool check ();
    void init ();
    void preHandle ();
private:
    friend class Cobic;
    Plane () {}//仅yon用来构造cobic中的plane数组。
    Vec3 _center, _normal, _dx, _dy;
    double _modelDx, _modelDy;
    //dx和dy只是为了指定纹理时使用，用来确定纹理的大小和方向
    double _dot_center_normal;

    double _d;
    bool _needBoudingBox;
};

class Triangle: public Object
{
public:
    Triangle (const Vec3& _a, const Vec3& b, const Vec3& c, Material* material = nullptr, Condutor* condutor = nullptr, bool need = false);
    Triangle (std::stringstream& content, Condutor* condutor = nullptr);
    virtual ~Triangle ();
    inline Vec3 a () {return _a;}
    inline Vec3 b () {return _b;}
    inline Vec3 c () {return _c;}
    Vec3 getRandomLink (const Vec3& source) const;//应当返回单位向量
    inline Vec3 normal () {return _normal;}
    Collide collide (const Ray &ray) const;
    virtual Color color (const Vec3 &v = Vec3 ()) const;
    void display (std::ostream& os) const;
protected:
    void analyseContent (std::stringstream& content);
    bool check ();
    void init ();
    void preHandle ();
private:
    Vec3 _a, _b, _c;
    Vec3 _normal;
    bool _needBoudingBox;
    Vec3 _dx, _dy;//for getRandomLink
};

class Cobic: public Object
{
public:
    Cobic (std::stringstream& content, Condutor* condutor = nullptr);
    Cobic (const Vec3& center, const Vec3& dx, const Vec3& dy, const Vec3& dz, double a, double b, double c, Material* material = nullptr, Condutor* condutor = nullptr, bool need = false);
    virtual ~Cobic ();
    Collide collide (const Ray& ray) const;
    virtual Color color (const Vec3 &v = Vec3 ()) const;
    void display (std::ostream& os) const;
    Vec3 getRandomLink (const Vec3& source) const;//应当返回单位向量
    //only for regular cubics
    double xh () const {return _x_h;}
    double xl () const {return _x_l;}
    double yh () const {return _y_h;}
    double yl () const {return _y_l;}
    double zh () const {return _z_h;}
    double zl () const {return _z_l;}
protected:
    void analyseContent (std::stringstream& content);
    bool check ();
    void init ();
    void preHandle ();
private:
    Vec3 _center, _dx, _dy, _dz;
    double _a, _b, _c;
    double _a_half, _b_half, _c_half;
    double _x_h, _x_l, _y_h, _y_l, _z_h, _z_l;
    Plane _sides[2][3];
    bool _needBoudingBox;
    bool _regular;
};

#endif // OBJECT_H
