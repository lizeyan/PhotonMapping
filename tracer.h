#ifndef TRACER_H
#define TRACER_H
#include "color.h"
#include "ray.h"
#include "object.h"
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
    void calcNearestCollide ();
protected:
    Ray _ray;
    Condutor* _condutor;
    Object* nearest;
    Collide collide;
    int _depth;
};
//=======inline functions=====================================
extern std::minstd_rand rd;
extern std::uniform_real_distribution<> rand01;
inline Vec3 Tracer::diffuse (const Vec3 &N)
{
    Vec3 x = vertical (N, Vec3()), y = cross (x, N);
    double cosPhi = rand01 (rd);
    double sinPhi = sqrt (1 - cosPhi * cosPhi);
    double xita = rand01(rd) * PI_Double;
    return cos (xita) * x * sinPhi + sin (xita) * y * sinPhi + cosPhi * N;
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

#endif // TRACER_H
