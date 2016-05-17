#include "photontracer.h"

PhotonTracer::PhotonTracer (const Ray& ray, Condutor* condutor): Tracer (condutor), _ray (ray)
{
    _ray.second = standardize (_ray.second);
}

void PhotonTracer::run ()
{

}
