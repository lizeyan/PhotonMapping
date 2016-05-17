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
    virtual ~RayTracer () {};
    void run ();
    Ray ray() {return _ray;}
    Color color () {return _color;}
protected:
    Color calcDiffusion (Light* light);
    void handleDiffusion ();
    void handleReflection ();
    void handleRefraction ();
    void calcNearestCollide ();
    void setColor (const Color& c) {_color = c;}
private:
    Ray _ray;
    Object* nearest;
    Collide collide;
    int _depth;
    Color _color;
};

#endif // RAYTRACER_H
