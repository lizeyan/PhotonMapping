#include "raytracer.h"
#include "condutor.h"
#include <utility>
RayTracer::RayTracer(const Ray& ray, Condutor* condutor): Tracer (condutor), _ray (ray)
{

}

Color RayTracer::run ()
{
    //find nearest object
    Collide collide;
    collide.distance = 1 << 30;
    Object* nearest = nullptr;
    for (const auto& object: condutor ()->object ())
    {
        Collide tmpCollide = object->collide (_ray);
        if (!collide.collide)
            continue;
        nearest = object.get ();
        if (tmpCollide.distance < collide.distance)
            collide = std::move(tmpCollide);
    }
    //no collide found
    if (nearest == nullptr)
    {
        return Color ();
    }
    Color resColor;
    //calc diffusion
    for (const auto& light: condutor ()->lights ())
    {
        resColor += calcDiffusion (collide, nearest, light.get ());
    }
    //calc reflection
    //calc refraction
    return resColor;
}

Color RayTracer::calcDiffusion (const Collide &collide, Object *object, Light *light)
{
    Color illuminate = light->illuminate (collide.point, collide.normal);
    Color material = object->material ()->color ();
    float diff = object->material ()->diffusion ();
    return diff * material * illuminate;
}
