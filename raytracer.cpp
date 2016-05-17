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

void RayTracer::calcNearestCollide ()
{
    nearest = nullptr;
    collide.distance = Bound;
    std::vector<std::pair<Object*, Collide> > potentialObs = condutor ()->kdTree ()->kdSearch (_ray);
    if (potentialObs.size())
    {
        collide.collide = true;
    }
    for (const auto& entry: potentialObs)
    {
        if (entry.second.distance < collide.distance)
        {
            collide = std::move (entry.second);
            nearest = entry.first;
        }
    }
#ifdef DEBUG
    Log << "depth:" << _depth << std::endl;
    if (nearest)
    {
        Log << "nearest:" << *nearest << std::endl;
        Log << "collide point:" << collide.point << std::endl;
    }
#endif
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
    /*
    Color resColor = color ();
    for (const auto& light: condutor ()->lights ())
    {
        resColor += calcDiffusion (light.get ());
    }
    setColor (resColor);
    */
    setColor (color () + condutor ()->photonMap ()->search (collide.point));
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
    Vec3 N = standardize (collide.normal);
    Vec3 I = standardize (_ray.second);
    double n = nearest->material ()->refractivity ();
    double cosi = dot(I, N);
    if (dot(N, I) < 0)
    {
        front = true;
        cosi = -cosi;
        I = -1 * I;
        n = 1 / n;
    }
    else
    {
        I = -1 * I;
        N = -1 * N;
    }
    double cost2 = 1 - n * n * (1 - cosi * cosi);
    Vec3 T;
    if (cost2 <= 0)//只有从光密介质到光疏介质才可能出现
        T = 2 * cosi * N - I;
    else
        T = -n * I + (n * cosi - sqrt (cost2)) * N;
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
