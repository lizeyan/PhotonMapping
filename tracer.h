#ifndef TRACER_H
#define TRACER_H
#include "color.h"
#include "ray.h"
#include "object.h"
#include "condutor.h"
#include <random>
class Condutor;
class Tracer
{
public:
    Tracer(const Ray& ray, Condutor* condutor, int depth);
    virtual ~Tracer () {};
    virtual void run () = 0;
    inline Condutor* condutor () const {return _condutor;}
    static Vec3 diffuse (const Vec3& N);
    static Vec3 reflect (const Vec3& I, const Vec3& N);
    static Vec3 refract (const Vec3& I, const Vec3& N, double n, bool& front);
protected:
	static void calcNearestCollide (const Ray& ray_, Object*& nearest_, Collide& collide_, Condutor* condutor);
protected:
    Ray _ray;
    Condutor* _condutor;
    Object* nearest;
    Collide collide;
    int _depth;
};
//=======inline functions=====================================
extern std::mt19937 rd;
extern std::uniform_real_distribution<> rand01;
inline Vec3 Tracer::diffuse (const Vec3 &N)
{
    Vec3 x = standardize (vertical (N, Vec3())), y = standardize (cross (x, N));
    if (fabs (model (N) - 1.0) > EPS)
        throw std::logic_error ("N is not a unit vector, in diffuse");
    double cosPhi2 = rand01 (rd);
    double sinPhi = sqrt (1.0 - cosPhi2);
    double xita = rand01(rd) * PI_Double;
    return cos (xita) * sinPhi * x + sin (xita) * sinPhi * y + sqrt (cosPhi2) * N;
}

inline Vec3 Tracer::reflect (const Vec3 &I, const Vec3 &N)
{
    return I - 2 * dot (I, N) * N;
}

inline Vec3 Tracer::refract (const Vec3 &I_, const Vec3 &N_, double n, bool& front)
{
    Vec3 I(I_), N(N_);
    double cosi = dot(I, N);
    if (dot(N, I) < 0)
    {
        front = true;
        cosi = -cosi;
        I = -1 * I;
        n = 1 / n;
    }
    else
    {
        front = false;
        I = -1 * I;
        N = -1 * N;
    }
    double cost2 = 1 - n * n * (1 - cosi * cosi);
    if (cost2 <= 0)//只有从光密介质到光疏介质才可能出现
        return 2 * cosi * N - I;
    else
        return -n * I + (n * cosi - sqrt (cost2)) * N;
}

inline void Tracer::calcNearestCollide (const Ray& ray_, Object*& nearest_, Collide& collide_, Condutor* condutor)
{
    nearest_ = nullptr;
    collide_.distance = Bound;
    std::vector<std::pair<Object*, Collide> > potentialObs = condutor->kdTree ()->kdSearch (ray_);
    if (potentialObs.size())
    {
        collide_.collide = true;
    }
    for (const auto& entry: potentialObs)
    {
        if (entry.second.distance < collide_.distance)
        {
            collide_ = std::move (entry.second);
            nearest_ = entry.first;
        }
    }
    if (collide_.collide)
    {
        collide_.normal = standardize (collide_.normal);
    }
#ifdef DEBUG
    Log << "depth:" << _depth << std::endl;
    if (nearest_)
    {
        Log << "nearest:" << *nearest_ << std::endl;
        Log << "collide point:" << collide_.point << std::endl;
    }
#endif
}


#endif // TRACER_H
