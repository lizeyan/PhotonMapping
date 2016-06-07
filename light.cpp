#include "condutor.h"
#include "light.h"
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
    Vec3 dir;
    do
    {
        dir[0] = rand01 (rd) * 2 - 1;
        dir[1] = rand01 (rd) * 2 - 1;
        dir[2] = rand01 (rd) * 2 - 1;
    }
    while (model2 (dir) > 1 || model2 (dir) < EPS);
    return Photon {_center, dir, color ()};
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
    double coefficient = dot (admitLight, normal);
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

bool PointLight::block (Object*& ob, const Vec3 &point, Condutor *condutor) const
{
    Vec3 link = standardize (_center - point);
    Ray ray = std::make_pair (point + EPS * link, link);
    std::vector<std::pair<Object*, Collide> > potentialObs = condutor->kdTree ()->kdSearch (ray);
    for (const auto& entry:potentialObs)
    {
        if (dot (link, _center - entry.second.point) > EPS && dot(link, point - entry.second.point) < -EPS)
        {
            ob = entry.first;
            return true;
        }
    }
    ob = nullptr;
    return false;
}
//==============================================
RectLight::RectLight (std::stringstream &content, Condutor *condutor)
{
    setCondutor (condutor);
    init ();
    analyseContent (content);
    if (!check ())
        throw std::logic_error ("invalid arguments, RectLight");
}
RectLight::RectLight (const Vec3 &center, const Vec3 &normal, const Vec3 &dx, const Vec3 &dy, double width, double height, const Color &color, Condutor *condutor):Light (color, condutor), _center (center), _normal (normal), _dx (dx), _dy (dy), _width (width), _height (height)
{

}

Photon RectLight::emitPhoton ()
{
    throw std::logic_error ("not completed");
    return Photon ();
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
    static std::uniform_real_distribution<> disX (0, _width);
    static std::uniform_real_distribution<> disY (0, _height);
    Vec3 rdPoint = ((_height / 2) - disY(rd)) * _dy + ((_width / 2) - disX (rd)) * _dx + _center;
    Vec3 admitLight = standardize(rdPoint - point);
    double coefficient = dot (admitLight, standardize(normal));
    if (coefficient < 0)
        return Color ();
    return coefficient * color ();
}

bool RectLight::block (Object*& ob, const Vec3 &point, Condutor *condutor) const
{
    static double halfX = _width / 2;
    static double halfY = _height / 2;
    static std::array<Vec3, 4> cornors{{Vec3(_center + halfX * _dx + halfY * _dy), Vec3(_center + halfX * _dx - halfY * _dy), Vec3(_center - halfX * _dx + halfY * _dy), Vec3(_center - halfX * _dx - halfY * _dy)}};
    for (unsigned int i = 0; i < cornors.size(); ++i)
    {
        Vec3 link = cornors[i] - point;
        Ray ray = std::make_pair (point, link);
        for (const auto& object:condutor->objects ())
        {
            if (object.get () == ob || ob->material ()->refraction () > EPS)
                continue;
            Collide collide = object->collide (ray);
            if (collide.collide && dot (link, _center - collide.point) > EPS && dot(link, point - collide.point) < -EPS)
            {
                return true;
            }
        }
    }
    return false;
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
    return fabs(dot (_dx, _normal)) < EPS && fabs (dot (_dy, _normal)) < EPS;
}

//===========================================
CircleLight::CircleLight (std::stringstream &content, Condutor *condutor)
{
    setCondutor (condutor);
    init ();
    analyseContent (content);
    if (!check ())
        throw std::logic_error ("invalid arguments, CircleLight");
}

CircleLight::CircleLight (const Vec3 &center, const Vec3 &normal, double radius, const Color &color, Condutor *condutor): Light(color, condutor), _center (center), _normal (normal), _radius (radius)
{

}

Photon CircleLight::emitPhoton ()
{
    throw std::logic_error ("not completed");
    return Photon ();
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
    Vec3 dx = vertical (_normal, _center);
    Vec3 dy = cross (_normal, dx);
    dx = standardize (dx);
    dy = standardize (dy);
    static std::uniform_real_distribution<> disR (0, _radius);
    double a, b, r2 = _radius * _radius;
    do
    {
        a =disR (rd);
        b = disR (rd);
    }
    while (a * a + b * b > r2);
    Vec3 rdPoint  = _center + dx * a + dy * b;
    Vec3 admitLight = standardize(rdPoint - point);
    double coefficient = dot (admitLight, normal);
    if (coefficient < 0)
        return Color ();
    return coefficient * color ();
}

bool CircleLight::block (Object*& ob, const Vec3 &point, Condutor *condutor) const
{
    Vec3 dx = vertical (_normal, _center);
    Vec3 dy = cross (_normal, dx);
    dx = standardize (dx);
    dy = standardize (dy);
    static std::array<Vec3, 4> cornors{{Vec3(_center + dx + dy), Vec3(_center + dx - dy), Vec3(_center - dx + dy), Vec3(_center - dx - dy)}};
    for (unsigned int i = 0; i < cornors.size(); ++i)
    {
        Vec3 link = standardize(cornors[i] - point);
        Ray ray = std::make_pair (point, link);
        for (const auto& object:condutor->objects ())
        {
            Collide collide = object->collide (ray);
            if (collide.collide && dot (link, _center - collide.point) > EPS && dot(link, point - collide.point) < -EPS)
            {
                ob = object.get ();
                return true;
            }
        }
    }
    ob = nullptr;
    return false;
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
    return _radius > 0 && model (_normal) > EPS;
}
