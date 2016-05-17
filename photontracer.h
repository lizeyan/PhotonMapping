#ifndef PHOTONTRACER_H
#define PHOTONTRACER_H
#include "tracer.h"
#include "ray.h"
#include "photonmap.h"
class PhotonTracer : public Tracer
{
public:
    PhotonTracer(const Ray& ray, Condutor* condutor);
    virtual ~PhotonTracer () {};
    void run ();
    Photon photon () {return _photon;}
private:
    Ray _ray;
    Photon _photon;
};

#endif // PHOTONTRACER_H
