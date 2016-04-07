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
    RayTracer(const Ray& ray, Condutor* condutor);
    Color run ();
    Ray ray() {return _ray;}
protected:
    Color calcDiffusion (const Collide& collide, Object* object, Light* light);
private:
    Ray _ray;
};

#endif // RAYTRACER_H
