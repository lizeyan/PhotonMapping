#ifndef PHOTONTRACER_H
#define PHOTONTRACER_H
#include "tracer.h"
#include "ray.h"
#include "photonmap.h"
class PhotonTracer : public Tracer
{
public:
    PhotonTracer(const Photon& photon, Condutor* condutor, int depth = 0, bool causitic = false);
    virtual ~PhotonTracer () {};
    void run ();
protected:
    bool handleDiffusion (double& prob);
    bool handleReflection (double& prob);
    bool handleRefraction (double& prob);
private:
    Color _color;
    PhotonMap* _photonMap;
    bool _causitic;//被追踪的光子如果是焦散光子，需要特殊对待。
};

#endif // PHOTONTRACER_H
