#include "tracer.h"
#include "condutor.h"
Tracer::Tracer(const Ray& ray, Condutor* condutor, int depth):_ray (ray), _condutor (condutor), nearest(nullptr), collide (), _depth (depth)
{
    _ray.second = standardize (_ray.second);
}

void Tracer::calcNearestCollide ()
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
    if (collide.collide)
    {
        collide.normal = standardize (collide.normal);
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

