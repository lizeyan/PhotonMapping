#ifndef CAMERA_H
#define CAMERA_H
#include "ray.h"
#include <string>
typedef Vector<3> Vec3;
class Camera
{
public:
    Camera (const std::string content);
    inline int width () {return _width;}
    inline int height () {return _height;}
    inline Vec3 center () {return _center;}
    inline Vec3 normal () {return _normal;}
    inline float focud () {return _focus;}
    Ray emitRay (int x, int y);
protected:
    void analyseContent (const std::string& content);
private:
    int _width;
    int _height;
    Vec3 _center, _normal;//中心，单位法向量
    float _focus;//感光点到image的距离
};

#endif // CAMERA_H
