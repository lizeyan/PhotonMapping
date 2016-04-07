#include "camera.h"
Camera::Camera(const std::string content)
{
    analyseContent (content);
}

Ray Camera::emitRay (int x, int y)
{
    Vec3 a = _center + _focus * _normal;
    Vec3 b =
}

void Camera::analyseContent (const std::string &content)
{

}

