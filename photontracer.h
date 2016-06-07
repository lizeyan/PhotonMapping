#ifndef PHOTONTRACER_H
#define PHOTONTRACER_H
#include "tracer.h"
#include "ray.h"
#include "photonmap.h"
class PhotonTracer : public Tracer
{
public:
    PhotonTracer(const Photon& ray, Condutor* condutor, int depth = 0);
    virtual ~PhotonTracer () {};
    void run ();
protected:
    bool handleDiffusion (double& prob);
    bool handleReflection (double& prob);
    bool handleRefraction (double& prob);
private:
    Color _color;
    PhotonMap* _photonMap;
};

#endif // PHOTONTRACER_H
