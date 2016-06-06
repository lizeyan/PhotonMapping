#include "tracer.h"
#include "condutor.h"
Tracer::Tracer(const Ray& ray, Condutor* condutor, int depth):_ray (ray), _condutor (condutor), nearest(nullptr), collide (), _depth (depth)
{
    _ray.second = standardize (_ray.second);
}
