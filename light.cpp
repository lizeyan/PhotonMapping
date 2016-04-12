#include "condutor.h"
#include "light.h"
#include "base.h"
#include <regex>
#include <cmath>
#include <utility>
#include <iostream>
#include <stdexcept>
Light* Light::produce (const std::string &content, Condutor* condutor)
{
    //必须首先指定类型
    std::string key;
    std::string value;
    std::string line;
    std::stringstream entryStream (content);
    std::smatch matchRes;
    while (!entryStream.eof())
    {
        std::getline (entryStream, line);
        if (!std::regex_match(line, matchRes, entryReg))
            continue;
        key = matchRes[keyRank].str ();
        value = matchRes[valueRank].str ();
        if (key != std::string("type"))
            continue;
        if (value == std::string("point"))
        {
            return new PointLight(entryStream, condutor);
        }
        else
            throw std::out_of_range("unkonwn light type:" + value);
    }
    throw std::logic_error("no valid type of light is given");
}

Light::Light(const Color& color, Condutor* condutor):Primitive(condutor), _color(color)
{

}
void Light::display (std::ostream &os) const
{
    os << "{";
    Primitive::display (os);
    os << "_color:" << _color << ";";
    os << "}";
}

//=======================================

PointLight::PointLight (const Vec3 &center, const Color &color, Condutor* condutor): Light(color, condutor), _center (center)
{

}

PointLight::PointLight (std::stringstream &content, Condutor* condutor)
{
    init ();
    setCondutor (condutor);
    analyseContent (content);
    if (!check ())
        throw std::logic_error ("incorrect point light argument");
}

void PointLight::init ()
{
    _center = Vec3 ();
}

bool PointLight::check ()
{
    return true;
}

void PointLight::analyseContent (std::stringstream &entryStream)
{
    std::smatch matchRes;
    std::string line;
    while (!entryStream.eof())
    {
        getline (entryStream, line);
        if (!std::regex_match(line, matchRes, entryReg))
            continue;
        std::string key = matchRes[keyRank].str ();
        std::string value = matchRes[valueRank].str ();
        if (key == std::string("color"))
        {
            Color c;
            std::stringstream valueStream (value);
            valueStream >> c[R] >> c[G] >> c[B];
            setColor (c);
        }
        else if (key == std::string ("center"))
        {
            Vec3 c;
            std::stringstream valueStream (value);
            valueStream >> c[0] >> c[1] >> c[2];
            _center = c;
        }
        else
        {
            throw std::logic_error("unexcepted key type in scene, point light");
        }
    }
}

Ray PointLight::emitPhoton ()
{
    return Ray ();
}

Collide PointLight::collide (const Ray &ray) const
{
    Vec3 line = ray.first - _center;
    Collide res;
    if (fabs(line[0]) < EPS && fabs(line[1]) < EPS  && fabs(line[2]) < EPS)
    {
        res.collide = true;
        res.distance = 0;
        res.point = _center;
        res.normal = Vec3 ();
        return res;
    }
    else
    {
        res.collide = false;
        if (_center == ray.first)
            res.distance = 0;
        else
        {
            res.distance = distance (_center, ray);
        }
        res.point = _center;
        res.normal = Vec3 ();
        return res;
    }
}

Color PointLight::illuminate (const Vec3 &point, const Vec3 &normal)
{
    Vec3 admitLight = standardize(_center - point);
    float coefficient = dot (admitLight, standardize(normal));
    return coefficient * color ();
}

void PointLight::display (std::ostream &os) const
{
    os << "{";
    Light::display (os);
    os << "_center:" << _center << ";";
    os << "}";
}

bool PointLight::block (Object* ob, const Vec3 &point, Condutor *condutor) const
{
    //std::cout << "point:" << point << " center:" << _center << std::endl;
    Vec3 link = _center - point;
    Ray ray = std::make_pair (point, link);
    for (const auto& object:condutor->objects ())
    {
        if (object.get () == ob || ob->material ()->refraction () > EPS)
            continue;
        Collide collide = object->collide (ray);
        if (collide.collide && dot (link, _center - collide.point) > EPS && dot(link, point - collide.point) < -EPS)
        {
            //std::cout << "collide point:" << collide.point << std::endl;
            return true;
        }
    }
    return false;
}
