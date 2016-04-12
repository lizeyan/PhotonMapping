#ifndef PHOTONTRACER_H
#define PHOTONTRACER_H

#include "tracer.h"
#include "condutor.h"
class PhotonTracer:public Tracer
{
public:
    PhotonTracer(Condutor* condutor);
};

#endif // PHOTONTRACER_H
