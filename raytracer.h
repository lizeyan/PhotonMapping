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
	Color getIndirect (const Collide &collide, Object* nearest);
    void handleDiffusion ();
    void handleReflection ();
    void handleRefraction ();
    void setColor (const Color& c) {_color = c;}
private:
    Color _color;
};

#endif // RAYTRACER_H
