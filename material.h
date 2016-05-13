#ifndef MATERIAL_H
#define MATERIAL_H
#include "color.h"
#include "image.h"
#include <string>
#include <iostream>
#include <memory>
class Material
{
public:
    static Material* produce (const std::string& content);
    Material(double diffusion  = 1, double reflection = 0, double refraction = 0, const Color& color = Color (), const Color& absorb = Color (), double refractivity = 1, Image* texture = nullptr);
    void display (std::ostream& os) const;
    inline double reflection () const {return _reflection;}
    inline double diffusion () const {return _diffusion;}
    inline double refraction () const {return _refraction;}
    inline double refractivity () const {return _refractivity;}
    inline Color color () const {return _color;}
    inline Color absorb () const {return _absorb;}
    Color color (double x, double y) const;
    Image* texture () {return _texture.get ();}
private:
    double _diffusion, _reflection, _refraction;
    double _refractivity;
    Color _color, _absorb;
    std::unique_ptr<Image> _texture;
};
std::ostream& operator<< (std::ostream& os, const Material& material);

#endif // MATERIAL_H
