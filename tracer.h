#ifndef TRACER_H
#define TRACER_H
#include "color.h"
class Condutor;
class Tracer
{
public:
    Tracer(Condutor* condutor);
    virtual ~Tracer () {};
    virtual void run () = 0;
    inline Condutor* condutor () const {return _condutor;}
private:
    Condutor* _condutor;
};

#endif // TRACER_H
