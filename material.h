#ifndef MATERIAL_H
#define MATERIAL_H
#include "color.h"
#include <string>
#include <iostream>
class Material
{
public:
    static Material* produce (const std::string& content);
    Material(float diffusion  = 1, float reflection = 0, float refraction = 0, const Color& color = Color (), const Color& absorb = Color (), float refractivity = 1);
    void display (std::ostream& os) const;
    inline float reflection () const {return _reflection;}
    inline float diffusion () const {return _diffusion;}
    inline float refraction () const {return _refraction;}
    inline float refractivity () const {return _refractivity;}
    inline Color color () {return _color;}
private:
    float _diffusion, _reflection, _refraction;
    float _refractivity;
    Color _color, _absorb;
};
std::ostream& operator<< (std::ostream& os, const Material& material);

#endif // MATERIAL_H
