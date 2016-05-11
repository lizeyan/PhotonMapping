#ifndef CAMERA_H
#define CAMERA_H
#include "ray.h"
#include "color.h"
#include <string>
#include <iostream>
typedef Vector<3> Vec3;
class Camera
{
public:
    Camera (const std::string content);
    void display (std::ostream& os) const;
    inline int width () {return _width;}
    inline int height () {return _height;}
    int dpi() {return _dpi;}
    inline Vec3 center () {return _center;}
    inline Vec3 normal () {return _normal;}
    inline Vec3 dx () {return _dx;}
    inline Vec3 dy () {return _dy;}
    inline Vec3 environment () {return _environment;}
    inline float focus () {return _focus;}
    Ray emitRay (float x, float y);
protected:
    void analyseContent (const std::string& content);
    void init ();
    bool check ();
    void preHandle ();
private:
    int _width;
    int _height;
    int _dpi;
    Vec3 _dx, _dy;//镜头x,y正方向的单位向量
    Vec3 _center, _normal;//中心，单位法向量
    float _focus;//感光点到image的距离
    Color _environment;
};
std::ostream& operator<< (std::ostream& os, const Camera& camera);
#endif // CAMERA_H
