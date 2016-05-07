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
RayTracer::RayTracer(const Ray& ray, Condutor* condutor, int depth): Tracer (condutor), _ray (ray), nearest(nullptr), collide (), _depth (depth)
{
}

Color RayTracer::run ()
{
    setColor (condutor ()->camera ()->environment ());
    //std::cout << "ray tracer, run" << std::endl;
    //find nearest object
    collide.distance = 1 << 30;
    for (const auto& object: condutor ()->objects ())
    {
        Collide tmpCollide = object->collide (_ray);
        if (!tmpCollide.collide)
            continue;
        if (tmpCollide.distance < collide.distance)
        {
            nearest = object.get ();
            collide = std::move(tmpCollide);
        }
    }
    //no collide found
    if (nearest == nullptr)
    {
        return color();
    }
//#ifdef DEBUG
//    Log << "nearest intersect object:" << *nearest << std::endl;
//    Log << "collide point of it:" << collide.point << " normal:" << collide.normal << std::endl;
//#endif
    //calc diffusion
    handleDiffusion ();
    if (_depth > MAX_RAY_TRACING_DEPTH)
        return color ();
    //calc reflection
    if (nearest->material ()->reflection () > EPS)
        handleReflection ();
    //calc refraction
    if (nearest->material ()->refraction () > EPS)
        handleRefraction ();
    return color ();
}

Color RayTracer::calcDiffusion (Light *light)
{
    Color illuminate = light->illuminate (collide.point, collide.normal);
    Color material = nearest->color (collide.point);
    float diff = nearest->material ()->diffusion ();
//#ifdef DEBUG
//    Log << "illuminate:" << illuminate << " material:" << material << " diff:" << diff << std::endl;
//#endif
    return diff * material * illuminate;
}

void RayTracer::handleDiffusion ()
{
    Color resColor = color ();
    for (const auto& light: condutor ()->lights ())
    {
        //test shadow
        if (light->block (nearest, collide.point, condutor ()))
            continue;
        resColor += calcDiffusion (light.get ());
    }
    setColor (resColor);
}

void RayTracer::handleReflection ()
{
    Color resColor = color ();
    Vec3 N = standardize (collide.normal);
    Vec3 I = standardize (_ray.second);
    if (dot(N, I) < -EPS)
    {
        Vec3 R = I - 2 * dot (I, N) * N;
        std::unique_ptr<RayTracer> reflTracer(new RayTracer(std::make_pair(collide.point + EPS * R, R), condutor (), _depth + 1));
        Color reflColor = reflTracer->run ();
        resColor += nearest->material ()->reflection () * reflColor * nearest->color (collide.point);
    }
    setColor (resColor);
}

void RayTracer::handleRefraction ()
{
    Color resColor = color ();
    Vec3 N = standardize (collide.normal);
    Vec3 I = standardize (_ray.second);
    double n = nearest->material ()->refractivity ();
    double cosi = dot(I, N);
    if (dot(N, I) < 0)
    {
        cosi = -cosi;
        I = -1 * I;
        n = 1 / n;
    }
    else
    {
        I = -1 * I;
        N = -1 * N;
    }
    double cost = sqrt (1 - n * n * (1 - cosi * cosi));
    Vec3 T = -n * I + (n * cosi - cost) * N;
    std::unique_ptr<RayTracer> refrTracer(new RayTracer(std::make_pair(collide.point + EPS * T, T), condutor (), _depth + 1));
    Color refrColor = refrTracer->run ();
//#ifdef DEBUG
//    if (_depth == 0)
//    {
//        Log << "T:" << T << std::endl;
//        Log << "refr Color:" << refrColor << std::endl;
//    }
//#endif
    resColor += nearest->material ()->refraction () * refrColor * nearest->material ()->absorb ();
    setColor (resColor);
}
