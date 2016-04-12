#ifndef RAYTRACER_H
#define RAYTRACER_H
#include "tracer.h"
#include "ray.h"
#include "color.h"
#include "object.h"
#include "light.h"
class Condutor;
class RayTracer: public Tracer
{
public:
    RayTracer(const Ray& ray, Condutor* condutor, int depth = 0);
    Color run ();
    Ray ray() {return _ray;}
protected:
    Color calcDiffusion (const Collide& collide, Object* object, Light* light);
    void handleDiffusion ();
    void handleReflection ();
    void handleRefraction ();
private:
    Object* nearest;
    Collide collide;
    Ray _ray;
    int _depth;
};

#endif // RAYTRACER_H
