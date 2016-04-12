#ifndef TRACER_H
#define TRACER_H
#include "color.h"
class Condutor;
class Tracer
{
public:
    Tracer(Condutor* condutor);
    virtual Color run () = 0;
    inline Condutor* condutor () const {return _condutor;}
    inline Color color () {return _color;}
    void setColor (const Color& c) {_color = c;}
private:
    Condutor* _condutor;
    Color _color;
};

#endif // TRACER_H
