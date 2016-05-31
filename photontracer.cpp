#include "photontracer.h"
#include "base.h"
#include "condutor.h"
#include <random>
extern std::mt19937 rd;
std::uniform_real_distribution<> rand01 (0, 1);
PhotonTracer::PhotonTracer (const Ray& ray, Condutor* condutor, const Color& color, int depth): Tracer (ray, condutor, depth), _color (color)
{
    _ray.second = standardize (_ray.second);
}

void PhotonTracer::run ()
{
    if (_depth > MAX_PHOTON_TRACING_DEPTH)
        return;
    calcNearestCollide ();
    if (nearest == nullptr)
        return;
    double prob = 1.0;
    if (!handleDiffusion (prob))
        if (!handleReflection (prob))
            handleRefraction (prob);
}

bool PhotonTracer::handleDiffusion (double &prob)
{
    _condutor->photonMap ()->store (Photon{collide.point, _ray.second, _color});

    double next = nearest->material ()->diffusion ();

    if (next <= rand01 (rd) * prob)
    {
        prob -= next;
        return false;
    }
    PhotonTracer pt(std::make_pair(collide.point, diffuse (collide.normal)), _condutor, _color * nearest->color (collide.point), _depth + 1);
    pt.run ();
    return true;
}

bool PhotonTracer::handleReflection (double &prob)
{
    double next = nearest->material ()->reflection ();
    if (next <= rand01 (rd) * prob)
    {
        prob -= next;
        return false;
    }
    PhotonTracer pt(std::make_pair(collide.point, reflect (_ray.second, collide.normal)), _condutor, _color * nearest->color (collide.point), _depth + 1);
    pt.run ();
    return true;
}

bool PhotonTracer::handleRefraction (double &prob)
{
    double next = nearest->material ()->refraction ();
    if (next <= rand01 (rd) * prob)
    {
        prob -= next;
        return false;
    }
    bool front;
    Vec3 T = refract (_ray.second, collide.normal, nearest->material ()->refractivity (), front);
    Color c (_color);
    if (!front)
    {
        Color absor = nearest->material ()->absorb () * -collide.distance;
        Color trans = Color (std::array<double, 3>{{std::exp (absor.arg (0)), std::exp (absor.arg (1)), std::exp (absor.arg (2))}} );
        c *= trans;
    }
    PhotonTracer pt (std::make_pair (collide.point, T), _condutor, c, _depth + 1);
    pt.run ();
    return true;
}

