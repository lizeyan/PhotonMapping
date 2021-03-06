#include "condutor.h"
#include "light.h"
#include "object.h"
#include "base.h"
#include <regex>
#include <cmath>
#include <utility>
#include <iostream>
#include <stdexcept>
extern std::mt19937 rd;
extern std::uniform_real_distribution<> rand01;
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
        else if (value == std::string("rectangle"))
        {
            return new RectLight (entryStream, condutor);
        }
        else if (value == std::string("circle"))
        {
            return new CircleLight (entryStream, condutor);
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

Photon PointLight::emitPhoton ()
{
    return Photon {_center, randomVector<3> (), color ()};
}
Photon PointLight::emitPhoton (Object* object)
{
    Vec3 dir = object->getRandomLink (_center);
    return Photon {_center, standardize (dir), color ()};
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
    Ray ray = std::make_pair (point + EPS * admitLight, admitLight);
    std::vector<std::pair<Object*, Collide> > potentialObs = condutor ()->kdTree ()->kdSearch (ray);
    double coefficient = dot (admitLight, normal);
    for (const auto& entry:potentialObs)
    {
        if (dot (admitLight, _center - entry.second.point) > 0.0 && dot(admitLight, point - entry.second.point) < 0.0)
            coefficient *= entry.first->material ()->refraction ();
    }
    if (coefficient < 0)
        coefficient = -coefficient;
    return coefficient * color ();
}

void PointLight::display (std::ostream &os) const
{
    os << "{";
    Light::display (os);
    os << "_center:" << _center << ";";
    os << "}";
}
//==============================================
RectLight::RectLight (std::stringstream &content, Condutor *condutor)
{
    setCondutor (condutor);
    init ();
    analyseContent (content);
    preHandle ();
    if (!check ())
        throw std::logic_error ("invalid arguments, RectLight");
}

void RectLight::preHandle ()
{
    _normal = standardize (_normal);
    _dx = standardize (_dx);
    _dy = standardize (_dy);
    _widthHalf = _width / 2;
    _heightHalf = _height / 2;
}

RectLight::RectLight (const Vec3 &center, const Vec3 &normal, const Vec3 &dx, const Vec3 &dy, double width, double height, const Color &color, Condutor *condutor):Light (color, condutor), _center (center), _normal (normal), _dx (dx), _dy (dy), _width (width), _height (height)
{
    preHandle ();
}

Photon RectLight::emitPhoton ()
{
    double x = rand01 (rd) * _width - _widthHalf;
    double y = rand01 (rd) * _height - _heightHalf;
    Vec3 dir = randomVector<3> ();
    //if (dot (dir, _normal) < 0.0)
        //dir = -1 * dir;
    return Photon {_center + x * _dx + y * _dy, dir, color ()};
}

Photon RectLight::emitPhoton (Object* object)
{
    double x = rand01 (rd) * _width - _widthHalf;
    double y = rand01 (rd) * _height - _heightHalf;
    Vec3 point = _center + x * _dx + y * _dy;
    return Photon {point, object->getRandomLink (point), color ()};
}

Collide RectLight::collide (const Ray &ray) const
{
//use triangle
    Vec3 halfX = (_width / 2) * _dx, halfY = (_height / 2) * _dy;
    Vec3 a = _center + halfX + halfY, b = _center + halfX - halfY;
    Vec3 c = _center - halfX + halfY, d = _center - halfX - halfY;
    std::unique_ptr<Triangle> t1, t2;
    t1.reset (new Triangle(a, d, b, nullptr, condutor ()));
    t2.reset (new Triangle(a, d, c, nullptr, condutor ()));
    Collide c1 = t1->collide (ray), c2 = t2->collide (ray);
    Collide res;
    res.collide = c1.collide || c2.collide;
    res.distance = std::min (c1.distance, c2.distance);
    if (c1.collide)
        res.point = c1.point;
    else if (c2.collide)
        res.point = c2.point;
    else
        res.point = Vec3 ();
    res.normal = _normal;
    return res;
}

Color RectLight::illuminate (const Vec3 &point, const Vec3 &normal)
{
    static double scale = 1.0 / static_cast<double> (_quality);
    Color resColor;
    for (size_t i = 0; i < _quality; ++i)
    {
        Vec3 rdPoint = _center + (rand01 (rd) * _width - _widthHalf) * _dx + (rand01(rd) * _height - _heightHalf) * _dy;
        Vec3 admitLight = standardize(rdPoint - point);
        std::vector<std::pair<Object*, Collide> > potentialObs = condutor ()->kdTree ()->kdSearch (std::make_pair (point + admitLight * EPS, admitLight));
        double coefficient = dot (admitLight, standardize(normal));
        for (const auto& ob: potentialObs)
        {
            if (dot (admitLight, _center - ob.second.point) > 0.0 && dot(admitLight, point - ob.second.point) < 0.0)
                coefficient *= ob.first->material ()->refraction ();
        }
        if (coefficient >= 0)
            resColor += coefficient * color ();
    }
    return scale * resColor;
}

void RectLight::display (std::ostream &os) const
{
    os << "{";
    Light::display (os);
    os << "_center:" << _center << ";";
    os << "_normal:" << _normal << ";";
    os << "_dx:" << _dx << ";";
    os << "_dy:" << _dy << ";";
    os << "_width:" << _width << ";";
    os << "_height:" << _height << ";";
    os << "}";
}

void RectLight::analyseContent (std::stringstream &entryStream)
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
        else if (key == std::string ("normal"))
        {
            Vec3 c;
            std::stringstream valueStream (value);
            valueStream >> c[0] >> c[1] >> c[2];
            _normal = c;
        }
        else if (key == std::string ("dx"))
        {
            Vec3 c;
            std::stringstream valueStream (value);
            valueStream >> c[0] >> c[1] >> c[2];
            _dx = c;
        }
        else if (key == std::string ("dy"))
        {
            Vec3 c;
            std::stringstream valueStream (value);
            valueStream >> c[0] >> c[1] >> c[2];
            _dy = c;
        }
        else if (key == std::string ("size"))
        {
            std::stringstream valueStream (value);
            valueStream >> _width >> _height;
        }
        else if (key == std::string ("quality"))
        {
            std::stringstream valueStream (value);
            valueStream >> _quality;
        }
        else
        {
            throw std::logic_error("unexcepted key type in scene, rectangle light");
        }
    }
}

void RectLight::init ()
{
    _center = Vec3 ();
    _normal = Vec3 (std::array<double, 3>{{0, 0, 1}});
    _dx = Vec3 (std::array<double, 3>{{1, 0, 0}});
    _dy = Vec3 (std::array<double, 3>{{0, 1, 0}});
    _width = 1;
    _height = 1;
}

bool RectLight::check ()
{
    return fabs(dot (_dx, _normal)) < EPS && fabs (dot (_dy, _normal)) < EPS && _quality >= 1 && model2 (_normal) >= EPS;
}

//===========================================
CircleLight::CircleLight (std::stringstream &content, Condutor *condutor)
{
    setCondutor (condutor);
    init ();
    analyseContent (content);
    preHandle ();
    if (!check ())
        throw std::logic_error ("invalid arguments, CircleLight");
}

CircleLight::CircleLight (const Vec3 &center, const Vec3 &normal, double radius, const Color &color, Condutor *condutor): Light(color, condutor), _center (center), _normal (normal), _radius (radius)
{
    preHandle ();
}

void CircleLight::preHandle ()
{
    _normal = standardize (_normal);
    _dx = standardize (vertical (_normal));
    _dy = cross (_normal, _dx);
}

Photon CircleLight::emitPhoton ()
{
    double x, y;
    do
    {
        x = rand01 (rd) * 2 - 1;
        y = rand01 (rd) * 2 - 1;
    }
    while (x * x + y * y > 1.0);
    Vec3 dir = randomVector<3> ();
    //if (dot (dir, _normal) < 0.0)
        //dir = -1 * dir;
    return Photon {_center + _dx * x * _radius + _dy * y * _radius, dir, color ()};
}
Photon CircleLight::emitPhoton (Object* object)
{
    double x, y;
    do
    {
        x = rand01 (rd) * 2 - 1;
        y = rand01 (rd) * 2 - 1;
    }
    while (x * x + y * y > 1.0);
    Vec3 point = _center + x * _dx * _radius + y * _dy * _radius;
    return Photon {point, object->getRandomLink (point), color ()};
}

Collide CircleLight::collide (const Ray &ray) const
{
    Collide collide = Plane (_center, _normal, nullptr, condutor ()).collide (ray);
    Collide res;
    res.distance = collide.distance;
    res.normal = collide.normal;
    if (collide.collide)
    {
        Vec3 link = collide.point - _center;
        double m = model (link);
        if (dot (link, _normal) < EPS && m <= _radius)
        {
            res.point = collide.point;
            res.collide = collide.collide;
        }
        else
        {
            res.point = Vec3 ();
            res.collide = false;
        }
    }
    else
    {
        res.point = collide.point;
        res.collide = collide.collide;
    }
    return res;
}

Color CircleLight::illuminate (const Vec3 &point, const Vec3 &normal)
{
    static double scale = 1.0 / static_cast<double> (_quality);
    Color resColor;
    for (size_t i = 0; i < _quality; ++i)
    {
        double x, y;
        do
        {
            x = rand01 (rd);
            y = rand01 (rd);
        }
        while (x * x + y * y > 1.0);
        Vec3 rdPoint = _center + x * _dx * _radius + y * _dy * _radius;
        Vec3 admitLight = standardize(rdPoint - point);
        std::vector<std::pair<Object*, Collide> > potentialObs = condutor ()->kdTree ()->kdSearch (std::make_pair (point + admitLight * EPS, admitLight));
        double coefficient = dot (admitLight, standardize(normal));
        for (const auto& ob: potentialObs)
        {
            if (dot (admitLight, _center - ob.second.point) > 0.0 && dot(admitLight, point - ob.second.point) < 0.0)
                coefficient *= ob.first->material ()->refraction ();
        }
        if (coefficient >= 0)
            resColor += coefficient * color ();
    }
    return scale * resColor;
}

void CircleLight::display (std::ostream &os) const
{
    os << "{";
    Light::display (os);
    os << "_center:" << _center << ";";
    os << "_normal:" << _normal << ";";
    os << "_radius:" << _radius << ";";
    os << "}";
}

void CircleLight::analyseContent (std::stringstream &entryStream)
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
        else if (key == std::string ("normal"))
        {
            Vec3 c;
            std::stringstream valueStream (value);
            valueStream >> c[0] >> c[1] >> c[2];
            _normal = c;
        }
        else if (key == std::string("radius"))
        {
            std::stringstream valueStream (value);
            valueStream >> _radius;
        }
        else if (key == std::string ("quality"))
        {
            std::stringstream valueStream (value);
            valueStream >> _quality;
        }
        else
        {
            throw std::logic_error("unexcepted key type in scene, rectangle light");
        }
    }
}

void CircleLight::init ()
{
    _center = Vec3 ();
    _normal = Vec3 (std::array<double, 3>{{0, 0, 1}});
    _radius = 1;
}

bool CircleLight::check ()
{
    return _radius > 0 && model (_normal) > EPS && _quality >= 1;
}
