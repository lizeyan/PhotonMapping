#ifndef OBJECT_H
#define OBJECT_H
#include "material.h"
#include "ray.h"
#include "primitive.h"
#include <string>
#include <memory>
#include <iostream>
typedef Vector<3> Vec3;
class Object: public Primitive
{
public:
    static Object* produce (const std::string& content, Condutor* condutor = nullptr);
    Object(Material* = nullptr, Condutor* condutor = nullptr);
    virtual Collide collide (const Ray& ray) const = 0;
    virtual void display (std::ostream& os) const;
    Material* material () {return _material;}
    inline void setMaterial (Material* m) {_material = m;}
    void calcBoudingBoxes ();
private:
    Material* _material;
    std::vector<Object*> _boudingBoxes;
};
class Sphere: public Object
{
public:
    Sphere (const Vec3& center, float radius, Material* material = nullptr, Condutor* condutor = nullptr);
    Sphere (std::stringstream& content, Condutor* condutor = nullptr);
    Vec3 center () const {return _center;}
    float radius () const {return _radius;}
    virtual Collide collide (const Ray &ray) const;
    virtual void display (std::ostream& os) const;
protected:
    void analyseContent (std::stringstream& content);
    bool check ();
    void init ();
private:
    Vec3 _center;
    float _radius;
};

class Plane: public Object
{
public:
    Plane (const Vec3& center, const Vec3& normal, Condutor* condutor = nullptr);
    Plane (std::stringstream& content, Condutor* condutor = nullptr);
    Vec3 center () const {return _center;}
    Vec3 normal () const {return _normal;}
    void display (std::ostream& os) const;
    Collide collide (const Ray &ray) const;
protected:
    void analyseContent (std::stringstream& content);
    bool check ();
    void init ();
private:
    Vec3 _center, _normal;
};

class Triangle: public Object
{
public:
    Triangle (const Vec3& a, const Vec3& b, const Vec3& c, Condutor* condutor = nullptr);
    Triangle (std::stringstream& content, Condutor* condutor = nullptr);
    inline Vec3 a () {return _a;}
    inline Vec3 b () {return _b;}
    inline Vec3 c () {return _c;}
    inline Vec3 normal () {return _normal;}
    Collide collide (const Ray &ray) const;
    void display (std::ostream& os) const;
protected:
    void analyseContent (std::stringstream& content);
    bool check ();
    void init ();
private:
    Vec3 _a, _b, _c;
    Vec3 _normal;
};

class Cobic: public Object
{
public:
    Cobic (std::stringstream& content, Condutor* condutor = nullptr);
    Cobic (const Vec3& center, const Vec3& dx, const Vec3& dy, float a, float b, float c, Condutor* condutor = nullptr);
    Collide collide (const Ray& ray) const;
    void display (std::ostream& os) const;
protected:
    void analyseContent (std::stringstream& content);
    bool check ();
    void init ();
private:
    Vec3 _center, _dx, _dy;
    float _a, _b, _c;
};

#endif // OBJECT_H
