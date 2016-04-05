#ifndef MATERIAL_H
#define MATERIAL_H


class Material
{
public:
    Material(float diffusion  = 1, float reflection = 0, float refraction = 0);
    inline float reflection () const {return _reflection;}
    inline float diffusion () const {return _diffusion;}
    inline float refraction () const {return _refraction;}
private:
    float _reflection, _diffusion, _refraction;
};

#endif // MATERIAL_H
