#include "condutor.h"
#include "object.h"
#include <cmath>
#include <iostream>
extern std::mt19937 rd;
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
        else if (value == std::string("cobic"))
        {
            return new Cobic (entryStream, condutor);
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

Color Sphere::color (const Vec3 &v) const
{
    static std::uniform_int_distribution<> choose(0, 1);
    if (v == Vec3 () || !this->material ()->texture ())
        return this->material ()->color ();
    else
    {
        Vec3 r = v - this->center ();
        float modelXY = sqrt (r.arg (0) * r.arg (0) + r.arg (1) * r.arg (1));
        float xita = atan (r.arg (2) / modelXY) + PI_Half;
        float phi = acos (r.arg (1) / modelXY);
        if (r.arg (0) < 0)
            phi += PI;
        if ((phi < EPS || PI_Double - phi < EPS ) && choose(rd))
            phi = PI_Double - phi;
        return this->material ()->color (xita / PI, phi / PI_Double);
    }
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
    float r2 = _radius * _radius;
    Collide res;
    if (d2 > r2 || (l2 > (r2 = _radius * _radius) && tp < 0))
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
        res.distance = 1 << 30;
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
Plane::Plane (const Vec3& center, const Vec3& normal, Material* material, Condutor* conductor):Object(material, conductor), _center (center), _normal (normal)
{
    unitize ();
}

Plane::Plane (std::stringstream &content, Condutor* condutor)
{
    init ();
    setCondutor (condutor);
    analyseContent (content);
    unitize ();
    if (!check ())
        throw std::logic_error ("incorrect argument of plane");
}

void Plane::unitize ()
{
    _normal = standardize (_normal);
}

Color Plane::color (const Vec3 &v) const
{
    if (v == Vec3 () || !this->material ()->texture ())
        return this->material ()->color ();
    else
    {
        Vec3 r = v - _center;
        float x = fabs(dot (r, _dx));
        float y = fabs(dot (r, _dy));
        return this->material ()->color ((x - _modelDx * int(x / _modelDx)) / _modelDx, (y - _modelDy * int(y / _modelDy)) / _modelDy);
    }
}

void Plane::init ()
{
    _center = Vec3 ();
    _dx = Vec3 ();
    _dy = Vec3 ();
    _normal = Vec3 (std::array<float, 3>{{0, 0, 1}});
}

bool Plane::check ()
{
    if (fabs(model (_normal)) < EPS || fabs(dot(_dx, _dy)) > EPS)
        return false;
    else if (material ()->texture () != nullptr && (_modelDx < EPS || _modelDy < EPS))
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
        else if (key == std::string ("dx"))
        {
            Vec3 n;
            std::stringstream valueStream (value);
            valueStream >> n[0] >> n[1] >> n[2];
            _dx = n;
            _modelDx = model (_dx);
        }
        else if (key == std::string ("dy"))
        {
            Vec3 n;
            std::stringstream valueStream (value);
            valueStream >> n[0] >> n[1] >> n[2];
            _dy = n;
            _modelDy = model (_dy);
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
    float rd_n = dot (rd, n);
    res.distance = 1 << 30;
    if (fabs(rd_n) < EPS)
        res.collide = false;
    else
    {
        res.distance = (dot(_center, _normal) - dot(_normal, ray.first)) / rd_n;
        res.point = ray.first + res.distance * rd;
        res.normal = n;
        if (res.distance > 0)
            res.collide = true;           
        else
            res.collide = false;
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
Triangle::Triangle (const Vec3 &a, const Vec3 &b, const Vec3 &c, Material* material, Condutor *condutor):Object (material, condutor), _a(a), _b(b), _c(c)
{
}

Triangle::Triangle (std::stringstream &content, Condutor *condutor)
{
    init ();
    setCondutor (condutor);
    analyseContent (content);
    if (!check ())
        throw std::logic_error ("incorrect argument of triangle");
}

Color Triangle::color (const Vec3 &v) const
{
    if (v == Vec3 () || !this->material ()->texture ())
        return this->material ()->color ();
    else
    {
        std::cout << "warning: incomplete color for triangle" << std::endl;
        return Color ();
    }
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
    if (_normal == Vec3 ())
        _normal = cross (_a - _b, _a - _c);
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
    Collide res;
    Vec3 e1 = _a - _b, e2 = _a - _c, r = standardize(ray.second);
    Vec3 S;
    float det1, det2, det3;
    float det0 = det (r, e1, e2);
    if (fabs (det0) < EPS)
    {
        goto fail;
    }
    S = _a - ray.first;
    det1 = det (S, e1, e2), det2 = det (r, S, e2), det3 = det (r, e1, S);
    if (det0 > 0 && (det1 < 0 || det2 + det3 > det0 || det2 < 0 || det3 < 0))
        goto fail;
    if (det0 < 0 && (det1 > 0 || det2 + det3 < det0 || det2 > 0 || det3 > 0))
        goto fail;
    res.collide = true;
    res.distance = det1 / det0;
    res.point = ray.first + res.distance * r;
    res.normal = _normal;
    return res;
    fail:
    {
        res.collide = false;
        res.distance = 1 << 30;
        res.normal = Vec3 ();
        res.point = Vec3 ();
        return res;
    }
}

//=========================================

Cobic::Cobic (std::stringstream &content, Condutor *condutor)
{
    setCondutor (condutor);
    init ();
    analyseContent (content);
    unitize ();
    if (!check ())
        throw std::logic_error ("invalid arguments, Cobic");
}
Cobic::Cobic (const Vec3 &center, const Vec3 &dx, const Vec3 &dy, const Vec3 &dz, float a, float b, float c, Material* material, Condutor *condutor): Object (material, condutor), _center (center), _dx(dx), _dy(dy), _dz (dz), _a(a), _b(b), _c(c), _a_half (_a / 2), _b_half (_b / 2), _c_half (_c / 2)
{
    unitize ();
}

void Cobic::unitize ()
{
    _dx = standardize (_dx);
    _dy = standardize (_dy);
    _dz = standardize (_dz);
}

Color Cobic::color (const Vec3 &v) const
{
    if (v == Vec3 () || !this->material ()->texture ())
        return this->material ()->color ();
    else
    {
        Vec3 r = v - _center;
        float x = fabs(dot (r, _dx));
        float y = fabs(dot (r, _dy));
        float z = fabs(dot (r, _dz));
        if (fabs (x - _a_half) < EPS)
        {
            return this->material ()->color (y / _b_half, z / _c_half);
        }
        else if (fabs (y - _b_half) < EPS)
        {
            return this->material ()->color (x / _a_half, z / _c_half);
        }
        else if (fabs (z - _c_half) < EPS)
        {
            return this->material ()->color (x / _a_half, y / _b_half);
        }
        else
            throw std::logic_error ("collide point not on cobic, in Cobic::color");
    }
}

Collide Cobic::collide (const Ray &ray) const
{
    Collide co1[3] = {Plane (_center + _a_half * _dx, _dx).collide (ray), Plane (_center + _b_half * _dy, _dy).collide (ray), Plane (_center + _c_half * _dz, _dz).collide (ray)}, co2[3] = {Plane (_center - _a_half * _dx, -1 * _dx).collide (ray), Plane (_center - _b_half * _dy,  -1 *_dy).collide (ray), Plane (_center - _c_half * _dz,  -1 *_dz).collide (ray)};
    for (int i = 0; i < 3; ++i)
        if (co1[i].distance > co2[i].distance)
        {
            std::swap (co1[i], co2[i]);
        }
    Collide *tmax1 = co1, *tmin2 = co2;
    for (int i = 1; i < 3; ++i)
    {
        if (tmax1->distance < co1[i].distance)
            tmax1 = &co1[i];
        if (tmin2->distance > co2[i].distance)
            tmin2 = &co2[i];
    }
    if (tmax1->distance > tmin2->distance)
    {
        Collide res;
        res.collide = false;
        res.distance = 1 << 30;
        return res;
    }
    else
    {
        return *tmax1;
    }
}

void Cobic::display (std::ostream &os) const
{
    os << "{";
    Object::display (os);
    os << "_center:" << _center << ";";
    os << "_dx:" << _dx << ";";
    os << "_dy:" << _dy << ";";
    os << "_dz:" << _dz << ";";
    os << "_a:" << _a << ";";
    os << "_b:" << _b << ";";
    os << "_c:" << _c << ";";
    os << "}";
}

void Cobic::analyseContent (std::stringstream &entryStream)
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
            std::stringstream valueStream (value);
            valueStream >> _a;
            _a_half = _a / 2;
        }
        else if (key == std::string ("b"))
        {
            std::stringstream valueStream (value);
            valueStream >>  _b;
            _b_half = _b / 2;
        }
        else if (key == std::string ("c"))
        {
            std::stringstream valueStream (value);
            valueStream >>  _c;
            _c_half = _c / 2;
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
        else if (key == std::string ("center"))
        {
            Vec3 n;
            std::stringstream valueStream (value);
            valueStream >> n[0] >> n[1] >> n[2];
            _center = n;
        }
        else if (key == std::string ("dx"))
        {
            Vec3 n;
            std::stringstream valueStream (value);
            valueStream >> n[0] >> n[1] >> n[2];
            _dx = n;
        }
        else if (key == std::string ("dy"))
        {
            Vec3 n;
            std::stringstream valueStream (value);
            valueStream >> n[0] >> n[1] >> n[2];
            _dy = n;
        }
        else if (key == std::string ("dz"))
        {
            Vec3 n;
            std::stringstream valueStream (value);
            valueStream >> n[0] >> n[1] >> n[2];
            _dz = n;
        }


        else
        {
            throw std::logic_error("unexcepted key type in scene, point light");
        }
    }
}

bool Cobic::check ()
{
    return _a > EPS && _b > EPS && _c > EPS;
}

void Cobic::init ()
{
    _center = Vec3 ();
    _dx = Vec3 (std::array<float, 3>{{1, 0 , 0}});
    _dy = Vec3 (std::array<float, 3>{{0, 1 , 0}});
    _dz = Vec3 (std::array<float, 3>{{0, 0 , 1}});
    _a = 0, _b = 0, _c = 0;
}
