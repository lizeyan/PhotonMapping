#include "condutor.h"
#include "object.h"
#include <cmath>
#include <iostream>
extern std::mt19937 rd;
extern std::uniform_real_distribution<> rand01;
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
Sphere::Sphere (const Vec3 &center, double radius, Material *material, Condutor* condutor, bool need): Object (material, condutor), _center (center), _radius (radius), _needBoudingBox (need)
{
    preHandle ();
}

Sphere::Sphere (std::stringstream &content, Condutor* condutor):_needBoudingBox (true)
{
    init ();
    setCondutor (condutor);
    analyseContent (content);
    preHandle ();
    if (!check ())
        throw std::logic_error ("incorrect argument of sphere");
}

void Sphere::preHandle ()
{
    _r2 = _radius * _radius;
    double rDouble = _radius * 2;
    if (_needBoudingBox)
        _boudingBox = (new Cobic (_center, unitX, unitY, unitZ, rDouble, rDouble, rDouble, material (), condutor ()));
}

Sphere::~Sphere ()
{
    if (_needBoudingBox)
        delete _boudingBox;
}

Color Sphere::color (const Vec3 &v) const
{
    static std::uniform_int_distribution<> choose(0, 1);
    if (v == Vec3 () || !this->material ()->texture ())
        return this->material ()->color ();
    else
    {
        Vec3 r = v - this->center ();
        double modelXY = sqrt (r.arg (0) * r.arg (0) + r.arg (1) * r.arg (1));
        double xita = atan (r.arg (2) / modelXY) + PI_Half;
        double phi = acos (r.arg (1) / modelXY);
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
    double l2 = dot (l, l);
    double tp = dot (l, ray.second);
    double tp2 = tp * tp;
    double d2 = l2 - tp2;
    Collide res;
    if (d2 > _r2 || (l2 > _r2 && tp < 0))
        res.collide = false;
    else
    {
        double t_2 = _r2 - d2;
        double t_ = sqrt (t_2);
        double t = tp;
        if (l2 < _r2)
            t += t_;
        else
            t -= t_;
        if (t > EPS)
        {
            res.collide = true;
            res.distance = t;
            res.point = ray.first + t * ray.second;
            res.normal = standardize (res.point - _center);
        }
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

Vec3 Sphere::getRandomLink (const Vec3 &source) const
{
    if (!_needBoudingBox)
        throw std::logic_error ("ask for a random link to a light from a object without boudingbox");
    Vec3 point;
    do
    {
        point[0] = rand01 (rd) * (_boudingBox->xh () - _boudingBox->xl ()) + _boudingBox->xl ();
        point[1] = rand01 (rd) * (_boudingBox->yh () - _boudingBox->yl ()) + _boudingBox->yl ();
        point[2] = rand01 (rd) * (_boudingBox->zh () - _boudingBox->zl ()) + _boudingBox->zl ();
    }
    while (distance2 (point, _center) > _r2);
    return standardize (point - source);
}
//=======================================================================================
Plane::Plane (const Vec3& center, const Vec3& normal, Material* material, Condutor* conductor, bool need):Object(material, conductor), _center (center), _normal (normal), _needBoudingBox (need)
{
    preHandle ();
}

Plane::Plane (std::stringstream &content, Condutor* condutor):_needBoudingBox (true)
{
    init ();
    setCondutor (condutor);
    analyseContent (content);
    preHandle ();
    if (!check ())
        throw std::logic_error ("incorrect argument of plane");
}

void Plane::preHandle ()
{
    _normal = standardize (_normal);
    _dot_center_normal = dot (_center, _normal);
    if (_needBoudingBox)
        _boudingBox = (new Cobic(_center, unitX, unitY, unitZ, Bound, Bound, Bound, material (), condutor ()));
    _d = -dot (_center, _normal);
}

inline double Plane::calc (const Vec3 &point) const
{
    return dot (point, _normal) + _d;
}

Plane::~Plane ()
{
    if (_needBoudingBox)
        delete _boudingBox;
}

Color Plane::color (const Vec3 &v) const
{
    if (v == Vec3 () || !this->material ()->texture ())
        return this->material ()->color ();
    else
    {
        Vec3 r = v - _center;
        double x = fabs(dot (r, _dx));
        double y = fabs(dot (r, _dy));
        double xRadio = (x - _modelDx * int(x / _modelDx)) / _modelDx;
        double yRadio = (y - _modelDy * int(y / _modelDy)) / _modelDy;
        return this->material ()->color (xRadio, yRadio);
    }
}

void Plane::init ()
{
    _center = Vec3 ();
    _dx = Vec3 ();
    _dy = Vec3 ();
    _normal = Vec3 (std::array<double, 3>{{0, 0, 1}});
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
    double rd_n = dot (ray.second, _normal);
    if (fabs(rd_n) >= EPS)
    {
        res.distance = (_dot_center_normal - dot(_normal, ray.first)) / rd_n;
        res.point = ray.first + res.distance * ray.second;
        res.normal = _normal;
        if (res.distance > 0.0)
            res.collide = true;
        //collide has been initialized to be false
    }
    return std::move(res);
}

void Plane::display (std::ostream &os) const
{
    os << "{";
    Object::display (os);
    os << "_center:" << _center << ";";
    os << "_normal:" << _normal << ";";
    os << "}";
}

Vec3 Plane::getRandomLink (const Vec3 &source) const
{
    Vec3 dir;
    do
    {
        dir[0] = rand01 (rd);
        dir[1] = rand01 (rd);
        dir[2] = rand01 (rd);
    }
    while (model2 (dir) > 1.0);
    double dir_v = dot (dir, _normal);
    double center_normal = dot (_center - source, _normal);
    if ((center_normal > 0 && dir_v > 0) || (center_normal < 0 && dir_v < 0))
        return dir;
    else
        return -1 * dir;
}
//======================================
Triangle::Triangle (const Vec3 &a, const Vec3 &b, const Vec3 &c, Material* material, Condutor *condutor, bool need):Object (material, condutor), _a(a), _b(b), _c(c), _normal (), _needBoudingBox (need)
{
    preHandle ();
}

Triangle::Triangle (std::stringstream &content, Condutor *condutor):_needBoudingBox (true)
{
    init ();
    setCondutor (condutor);
    analyseContent (content);
    preHandle ();
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

void Triangle::preHandle ()
{
    if (_normal == Vec3 ())
    {
        _normal = standardize (cross (_a - _b, _a - _c));
    }
    if (_needBoudingBox)
    {
        double maxX = _a.arg (0), maxY = _a.arg (1), maxZ = _a.arg (2);
        double minX = _a.arg (0), minY = _a.arg (1), minZ = _a.arg (2);
        if (_b.arg (0) > maxX)
            maxX = _b.arg (0);
        else if (_b.arg (0) < minX)
            minX = _b.arg (0);
        if (_c.arg (0) > maxX)
            maxX = _c.arg (0);
        else if (_c.arg (0) < minX)
            minX = _c.arg (0);
        if (_b.arg (1) > maxY)
            maxY = _b.arg (1);
        else if (_b.arg (1) < minY)
            minY = _b.arg (1);
        if (_c.arg (1) > maxY)
            maxY = _c.arg (1);
        else if (_c.arg (1) < minY)
            minY = _c.arg (1);
        if (_b.arg (2) > maxZ)
            maxZ = _b.arg (2);
        else if (_b.arg (2) < minZ)
            minZ = _b.arg (2);
        if (_c.arg (2) > maxZ)
            maxZ = _c.arg (2);
        else if (_c.arg (2) < minZ)
            minZ = _c.arg (2);
        _boudingBox = (new Cobic (Vec3(std::array<double,3> {{(maxX + minX) / 2, (maxY + minY) / 2, (maxZ + minZ) / 2}}), unitX, unitY, unitZ, maxX - minX, maxY - minY, maxZ - minZ, material (), condutor ()));
    }
    _dx = _b - _a;
    _dy = standardize (cross (_normal, _dx));
    _dy *= dot (_dy, _c - _a);
}
Triangle::~Triangle ()
{
    if (_needBoudingBox)
        delete _boudingBox;
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
    Vec3 e1 = _a - _b, e2 = _a - _c;
    Vec3 S;
    double det1, det2, det3;
    double det0 = det (ray.second, e1, e2);
    if (fabs (det0) < EPS)
        goto fail;
    S = _a - ray.first;
    det1 = det (S, e1, e2), det2 = det (ray.second, S, e2), det3 = det (ray.second, e1, S);
    if (det0 > 0.0 && (det1 < 0.0 || det2 + det3 > det0 || det2 < 0.0 || det3 < 0.0))
        goto fail;
    if (det0 < 0.0 && (det1 > 0.0 || det2 + det3 < det0 || det2 > 0.0 || det3 > 0.0))
        goto fail;
    res.collide = true;
    res.distance = det1 / det0;
    res.point = ray.first + res.distance * ray.second;
    res.normal = _normal;
    return res;
    fail:
    {
        return res;
    }
}

Vec3 Triangle::getRandomLink (const Vec3 &source) const
{
    return _a + rand01 (rd) * _dx + rand01 (rd) * _dy - source;
}
//=========================================

Cobic::Cobic (std::stringstream &content, Condutor *condutor):_needBoudingBox (true)
{
    setCondutor (condutor);
    init ();
    analyseContent (content);
    preHandle ();
    if (!check ())
        throw std::logic_error ("invalid arguments, Cobic");
}
Cobic::Cobic (const Vec3 &center, const Vec3 &dx, const Vec3 &dy, const Vec3 &dz, double a, double b, double c, Material* material, Condutor *condutor, bool need): Object (material, condutor), _center (center), _dx(dx), _dy(dy), _dz (dz), _a(a), _b(b), _c(c), _a_half (_a / 2), _b_half (_b / 2), _c_half (_c / 2), _needBoudingBox (need)
{
    preHandle ();
}

Cobic::~Cobic ()
{

}

void Cobic::preHandle ()
{
    _dx = standardize (_dx);
    _dy = standardize (_dy);
    _dz = standardize (_dz);
    if (_dx == unitX && _dy == unitY && _dz == unitZ)
    {
        _regular = true;
        _x_h = _center.arg (0) + _a_half;
        _x_l = _center.arg (0) - _a_half;
        _y_h = _center.arg (1) + _b_half;
        _y_l = _center.arg (1) - _b_half;
        _z_h = _center.arg (2) + _c_half;
        _z_l = _center.arg (2) - _c_half;
    }
    else
        _regular = false;
    if (_needBoudingBox)
        _boudingBox = this;
    _sides[0][0] = Plane (_center + _a_half * _dx, _dx);
    _sides[1][0] = Plane (_center - _a_half * _dx, -1 * _dx);
    _sides[0][1] = Plane (_center + _b_half * _dy, _dy);
    _sides[1][1] = Plane (_center - _b_half * _dy,  -1 *_dy);
    _sides[0][2] = Plane (_center + _c_half * _dz, _dz);
    _sides[1][2] = Plane (_center - _c_half * _dz,  -1 *_dz);
}

Color Cobic::color (const Vec3 &v) const
{
    if (v == Vec3 () || !this->material ()->texture ())
        return this->material ()->color ();
    else
    {
        Vec3 r = v - _center;
        double x = fabs(dot (r, _dx));
        double y = fabs(dot (r, _dy));
        double z = fabs(dot (r, _dz));
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
    Collide co[2][3];
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 3; ++j)
            co[i][j] = _sides[i][j].collide (ray);
    for (int i = 0; i < 3; ++i)
    {
        if (co[0][i].distance > co[1][i].distance)
        {
            std::swap (co[0][i], co[1][i]);
        }
        //此时光线和这一组边平行
        if (co[0][i].distance == Bound && co[1][i].distance == Bound)
        {
            double d1 = _sides[0][i].calc (ray.first), d2 = _sides[1][i].calc (ray.first);
            if ((d1 > 0.0 && d2 > 0.0) || (d1 < 0.0 && d2 < 0.0))
            {
                co[0][i].distance = -Bound;
            }
        }
    }
    Collide *tmax1 = co[0], *tmin2 = co[1];
    for (int i = 1; i < 3; ++i)
    {
        if (tmax1->distance < co[0][i].distance)
            tmax1 = co[0] + i;
        if (tmin2->distance > co[1][i].distance)
            tmin2 = co[1] + i;
    }
    if (tmax1->distance > tmin2->distance)
    {
        return std::move (Collide ());
    }
    else
    {
        //tmax1->collide = co1[rankMax].collide || co2[rankMax].collide;
        if (tmax1->collide)
            return *tmax1;
        else//说明co[0]中所有的distance都小于0
            return *tmin2;
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
    _dx = Vec3 (std::array<double, 3>{{1, 0 , 0}});
    _dy = Vec3 (std::array<double, 3>{{0, 1 , 0}});
    _dz = Vec3 (std::array<double, 3>{{0, 0 , 1}});
    _a = 0, _b = 0, _c = 0;
}

Vec3 Cobic::getRandomLink (const Vec3 &source) const
{
    Vec3 dir;
    dir[0] = rand01(rd) * (xh () - xl ()) + xl ();
    dir[1] = rand01(rd) * (yh () - yl ()) + yl ();
    dir[2] = rand01(rd) * (zh () - zl ()) + zl ();
    return dir - source;
}
