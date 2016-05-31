#include "raytracer.h"
#include "condutor.h"
#include <utility>
#include <iostream>
#include <cmath>
#include <thread>
#include <mutex>
#ifdef DEBUG
extern std::ofstream Log;
#endif
RayTracer::RayTracer(const Ray& ray, Condutor* condutor, int depth): Tracer (ray, condutor, depth)
{
}

void RayTracer::run ()
{
    setColor (condutor ()->camera ()->environment ());
    calcNearestCollide ();
    //no collide found
    if (nearest == nullptr)
        return;
//#ifdef DEBUG
//    Log << "nearest intersect object:" << *nearest << std::endl;
//    Log << "collide point of it:" << collide.point << " normal:" << collide.normal << std::endl;
//#endif
    //calc diffusion
    if (nearest->material ()->diffusion () > EPS)
        handleDiffusion ();
    if (_depth > MAX_RAY_TRACING_DEPTH)
        return;
    //calc reflection
    if (nearest->material ()->reflection () > EPS)
        handleReflection ();
    //calc refraction
    if (nearest->material ()->refraction () > EPS)
        handleRefraction ();
}
Color RayTracer::calcDiffusion (Light *light)
{
    Color illuminate;
    Object* co;
    if (light->block (co, collide.point, condutor ()))
        illuminate = co->material ()->refraction () * light->color ();
    else
        illuminate = light->illuminate (collide.point, collide.normal);
    Color material = nearest->color (collide.point);
    double diff = nearest->material ()->diffusion ();
//#ifdef DEBUG
//    Log << "illuminate:" << illuminate << " material:" << material << " diff:" << diff << std::endl;
//#endif
    return diff * material * illuminate;
}

void RayTracer::handleDiffusion ()
{
#ifdef PHOTON_MAPPING
    auto resPair = condutor ()->photonMap ()->search (collide.point);
    std::vector<Photon*> photons = std::move(resPair.first);
    double radius = resPair.second;
    Color resColor;
    for (const auto& p: photons)
    {
        double coef = dot (p->dir, collide.normal);
        if (coef >= 0.0)
            continue;
        resColor += -coef * p->color;
    }
//    double scale = nearest->material ()->diffusion () / (double(photons.size ()) * PI * radius * radius);
    double scale = nearest->material ()->diffusion () / double(photons.size ());
    resColor *= scale;
    resColor *= nearest->color (collide.point);
    setColor (resColor + color ());
#else
    Color resColor = color ();
    for (const auto& light: condutor ()->lights ())
    {
        resColor += calcDiffusion (light.get ());
    }
    setColor (resColor);
#endif
}

void RayTracer::handleReflection ()
{
    Color resColor = color ();
    Vec3 N = collide.normal;
    Vec3 I = _ray.second;
    if (dot(N, I) < -EPS)
    {
        Vec3 R = reflect (I, N);
        std::unique_ptr<RayTracer> reflTracer(new RayTracer(std::make_pair(collide.point + EPS * R, R), condutor (), _depth + 1));
        reflTracer->run ();
        Color reflColor = reflTracer->color ();
        resColor += nearest->material ()->reflection () * reflColor * nearest->color (collide.point);
    }
    setColor (resColor);
}

void RayTracer::handleRefraction ()
{
    bool front = false;
    Color resColor = color ();
    Vec3 T = refract (_ray.second, collide.normal, nearest->material ()->refractivity (), front);
    std::unique_ptr<RayTracer> refrTracer(new RayTracer(std::make_pair(collide.point + EPS * T, T), condutor (), _depth + 1));
    refrTracer->run ();
    Color refrColor = refrTracer->color ();
//#ifdef DEBUG
//    if (_depth == 0)
//    {
//        Log << "T:" << T << std::endl;
//        Log << "refr Color:" << refrColor << std::endl;
//    }
//#endif
    if (front)
        resColor += nearest->material ()->refraction () * refrColor;
    else
    {
        Color absor = nearest->material ()->absorb () * -collide.distance;
        Color trans = Color (std::array<double, 3>{{std::exp (absor.arg (0)), std::exp (absor.arg (1)), std::exp (absor.arg (2))}} );
        resColor += nearest->material ()->refraction () * refrColor * trans;
    }
    setColor (resColor);
}
