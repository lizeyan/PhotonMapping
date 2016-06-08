#include "material.h"
#include <stdexcept>
Material::Material (double diffusion ,double reflection ,double refraction, const Color& color, const Color& absorb, double refractivity, Image* texture):_diffusion (diffusion), _reflection(reflection), _refraction (refraction), _refractivity(refractivity), _color (color), _absorb (absorb), _texture (texture)
{
    if (_refractivity < EPS)
        throw std::logic_error("zero refractivity");
	_color = standardize (_color);
}

Material* Material::produce (const std::string &content)
{
    std::stringstream entryStream (content);
    std::smatch matchRes;
    std::string line;
    Color color, absorb;
    double diffusion = 1, reflection = 0, refraction = 0;
    double refractivity = 1;
    Image* texture = nullptr;
    while (!entryStream.eof())
    {
        getline (entryStream, line);
        if (!std::regex_match(line, matchRes, entryReg))
            continue;
        std::string key = matchRes[keyRank].str ();
        std::string value = matchRes[valueRank].str ();
        std::stringstream valueStream (value);
        if (key == std::string("color"))
        {
            valueStream >> color[R] >> color[G] >> color[B];
        }
        else if (key == std::string("absorb"))
        {
            valueStream >> absorb[R] >> absorb[G] >> absorb[B];
        }
        else if (key == std::string("diffusion"))
        {
            valueStream >> diffusion;
        }
        else if (key == std::string("refractivity"))
        {
            valueStream >> refractivity;
        }
        else if (key == std::string("reflection"))
        {
            valueStream >> reflection;
        }
        else if (key == std::string("refraction"))
        {
            valueStream >> refraction;
        }
        else if (key == std::string ("texture"))
        {
            texture = (new Image(value));
        }
        else
        {
            throw std::logic_error("unexcepted key type in scene, material");
        }
    }
    return new Material(diffusion, reflection, refraction, color, absorb, refractivity, texture);
}

void Material::display (std::ostream &os) const
{
    os << "{";
    os << "_diffusion:" << _diffusion << ";";
    os << "_reflection:" << _reflection << ";";
    os << "-refraction:" << _refraction << ";";
    os << "_color:" << _color << ";";
    os << "_absorb:" << _absorb << ";";
    os << "}";
}

std::ostream& operator << (std::ostream& os, const Material& material)
{
    material.display (os);
    return os;
}

Color Material::color (double x, double y) const
{
    if (!_texture)
        throw std::logic_error ("unaccessable memory of _texture");
    return _texture->smoothPixel (x, y);
}
