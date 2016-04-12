#include "condutor.h"
#include "object.h"
#include <cmath>
#include <iostream>
Object* Object::produce (const std::string &content, Condutor* condutor)
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
        if (value == std::string("sphere"))
        {
            return new Sphere(entryStream, condutor);
        }
        else if (value == std::string("plane"))
        {
            return new Plane(entryStream, condutor);
        }
        else if (value == std::string("triangle"))
        {
            return new Triangle (entryStream, condutor);
        }
        else
            throw std::out_of_range("unkonwn object type:" + value);
    }
    throw std::logic_error("no valid type of object is given");
}

Object::Object(Material* material, Condutor* condutor):Primitive(condutor), _material (material)
{
}

void Object::display (std::ostream &os) const
{
    os << "{";
    Primitive::display (os);
    os << "_material:" << _material << ";";
    os << "}";
}

//=======================================================================================
Sphere::Sphere (const Vec3 &center, float radius, Material *material, Condutor* condutor): Object (material, condutor), _center (center), _radius (radius)
{

}

Sphere::Sphere (std::stringstream &content, Condutor* condutor)
{
    init ();
    setCondutor (condutor);
    analyseContent (content);
    if (!check ())
        throw std::logic_error ("incorrect argument of sphere");
}

void Sphere::init ()
{
    _center = Vec3 ();
    _radius = 1;
}

bool Sphere::check ()
{
    if (fabs(_radius) < EPS)
        return false;
    else
        return true;
}

void Sphere::analyseContent (std::stringstream &entryStream)
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

        if (key == std::string ("center"))
        {
            Vec3 c;
            std::stringstream valueStream (value);
            valueStream >> c[0] >> c[1] >> c[2];
            _center = c;
        }
        else if (key == std::string ("material"))
        {
            int materialRank = 0;
            std::stringstream valueStream (value);
            valueStream >> materialRank;
            if (condutor () == nullptr)
                throw std::logic_error("access a null pointer of Condutor");
            setMaterial (condutor ()->materials ().at (materialRank).get ());
        }
        else if (key == std::string ("radius"))
        {
            std::stringstream valueStream (value);
            valueStream >> _radius;
        }
        else
        {
            throw std::logic_error("unexcepted key type in scene, point light");
        }
    }
}

Collide Sphere::collide (const Ray &ray) const
{
    Vec3 l = _center - ray.first;
    float l2 = dot (l, l);
    Vec3 rd = standardize (ray.second);
    float tp = dot (l, rd);
    float tp2 = tp * tp;
    float d2 = l2 - tp2;
    float d = sqrt (d2);
    float r2 = 0;
    Collide res;
    res.distance = d;
    if (d > _radius || (l2 > (r2 = _radius * _radius) && tp < 0))
    {
        res.collide = false;
    }
    else
    {
        float t_2 = r2 - d2;
        float t_ = sqrt (t_2);
        float t = tp;
        if (l2 < r2)
            t += t_;
        else
            t -= t_;
        if (t > EPS)
        {
            res.collide = true;
            res.distance = t;
            res.point = ray.first + t * rd;
            res.normal = res.point - _center;
        }
        else
        {
            res.collide = false;
        }
    }

    if (res.collide == false)
    {
        res.point = Vec3 ();
        res.normal = Vec3 ();
    }
    return std::move (res);
}

void Sphere::display (std::ostream &os) const
{
    os << "{";
    Object::display (os);
    os << "_center:" << _center << ";";
    os << "_radius:" << _radius << ";";
    os << "}";
}

//=======================================================================================
Plane::Plane (const Vec3& center, const Vec3& normal, Condutor* condutor): _center (center), _normal (normal)
{
    setCondutor (condutor);

}

Plane::Plane (std::stringstream &content, Condutor* condutor)
{
    init ();
    setCondutor (condutor);
    analyseContent (content);
    if (!check ())
        throw std::logic_error ("incorrect argument of plane");
}

void Plane::init ()
{
    _center = Vec3 ();
    _normal = Vec3 (std::array<float, 3>{{0, 0, 1}});
}

bool Plane::check ()
{
    if (fabs(model (_normal)) < EPS)
        return false;
    else
        return true;
}

void Plane::analyseContent (std::stringstream &entryStream)
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

        if (key == std::string ("center"))
        {
            Vec3 c;
            std::stringstream valueStream (value);
            valueStream >> c[0] >> c[1] >> c[2];
            _center = c;
        }
        else if (key == std::string ("material"))
        {
            int materialRank = 0;
            std::stringstream valueStream (value);
            valueStream >> materialRank;
            if (condutor () == nullptr)
                throw std::logic_error("access a null pointer of Condutor");
            setMaterial (condutor()->materials ().at (materialRank).get ());
        }
        else if (key == std::string ("normal"))
        {
            Vec3 n;
            std::stringstream valueStream (value);
            valueStream >> n[0] >> n[1] >> n[2];
            _normal = n;
        }
        else
        {
            throw std::logic_error("unexcepted key type in scene, point light");
        }
    }
}

Collide Plane::collide (const Ray &ray) const
{
    Collide res;
    Vec3 rd = standardize (ray.second);
    Vec3 n = standardize (_normal);
    float d = - dot (n, _center);
    float n_ro = dot (n, ray.first);
    res.distance = fabs (n_ro + d);
    float rd_n = dot (rd, n);
    if (-EPS < rd_n && rd_n < EPS)
    {
        res.collide = false;
    }
    else
    {
        float t = - (d + n_ro) / rd_n;
        if (t > EPS)
        {
            res.collide = true;
            res.point = ray.first + t * rd;
            res.normal = n;
            res.distance = t;
        }
        else
        {
            res.collide = false;
        }
    }

    if (res.collide == false)
    {
        res.point = Vec3 ();
        res.normal = Vec3 ();
    }
    return res;
}

void Plane::display (std::ostream &os) const
{
    os << "{";
    Object::display (os);
    os << "_center:" << _center << ";";
    os << "_normal:" << _normal << ";";
    os << "}";
}

//======================================
Triangle::Triangle (const Vec3 &a, const Vec3 &b, const Vec3 &c, Condutor *condutor): _a(a), _b(b), _c(c)
{
    setCondutor (condutor);
}

Triangle::Triangle (std::stringstream &content, Condutor *condutor)
{
    init ();
    setCondutor (condutor);
    analyseContent (content);
    if (!check ())
        throw std::logic_error ("incorrect argument of triangle");
}

void Triangle::init ()
{
    _a = Vec3 ();
    _b = Vec3 ();
    _c = Vec3 ();
    _normal = Vec3 ();
}

bool Triangle::check ()
{
    if (_a == _b || _b == _c || _a == _c || _normal == Vec3 ())
        return false;
    else
        return true;
}

void Triangle::analyseContent (std::stringstream &entryStream)
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

        if (key == std::string ("a"))
        {
            Vec3 a_;
            std::stringstream valueStream (value);
            valueStream >> a_[0] >> a_[1] >> a_[2];
            _a = a_;
        }
        else if (key == std::string ("b"))
        {
            Vec3 b_;
            std::stringstream valueStream (value);
            valueStream >> b_[0] >> b_[1] >> b_[2];
            _b = b_;
        }
        else if (key == std::string ("c"))
        {
            Vec3 c_;
            std::stringstream valueStream (value);
            valueStream >> c_[0] >> c_[1] >> c_[2];
            _c = c_;
        }

        else if (key == std::string ("material"))
        {
            int materialRank = 0;
            std::stringstream valueStream (value);
            valueStream >> materialRank;
            if (condutor () == nullptr)
                throw std::logic_error("access a null pointer of Condutor");
            setMaterial (condutor()->materials ().at (materialRank).get ());
        }
        else if (key == std::string ("normal"))
        {
            Vec3 n;
            std::stringstream valueStream (value);
            valueStream >> n[0] >> n[1] >> n[2];
            _normal = n;
        }
        else
        {
            throw std::logic_error("unexcepted key type in scene, point light");
        }
    }
}

void Triangle::display (std::ostream& os) const
{
    os << "{";
    Object::display (os);
    os << "_a:" << _a << ";";
    os << "_b:" << _b << ";";
    os << "_c:" << _c << ";";
    os << "_normal:" << _normal << ";";
    os << "}";
}

Collide Triangle::collide (const Ray &ray) const
{

}
