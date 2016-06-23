#ifndef VECTOR_H
#define VECTOR_H
#include "base.h"
#include <array>
#include <utility>
#include <cmath>
#include <stdexcept>
template <std::size_t N>
class Vector
{
public:
    Vector (const std::array<double, N>& = std::array<double, N>());
    double arg (unsigned int n) const;
    double& operator[] (unsigned int n);
    void setArg (int n, double newValue);
    Vector<N>& operator += (const Vector<N>& a);
    Vector<N>& operator -= (const Vector<N>& a);
    Vector<N>& operator *= (const Vector<N>& a);
    Vector<N>& operator *= (double factor);
private:
    std::array<double, N> _args;
};
typedef Vector<3> Vec3;
template <std::size_t N>
std::ostream& operator<< (std::ostream& os, const Vector<N>& vec)
{
    os << "(" << vec.arg (0);
    for (unsigned int i = 1; i < N; ++i)
        os << "," << vec.arg(i);
    os << ")";
    return os;
}
template <std::size_t N>
Vector<N>::Vector (const std::array<double, N> &args):_args(args)
{
}
template <std::size_t N>
inline double Vector<N>::arg (unsigned int n) const
{
    return _args[n];
}
template <std::size_t N>
inline double& Vector<N>::operator [] (unsigned int n)
{
    return _args[n];
}

template <std::size_t N>
inline void Vector<N>::setArg (int n, double newValue)
{
    _args[n] = newValue;
}

template <std::size_t N>
inline double dot (const Vector<N>& a, const Vector<N>& b)
{
    double res = 0;
    for (unsigned int i = 0; i < N; ++i)
        res += (a.arg (i) * b.arg (i));
    return res;
}
template <std::size_t N>
Vector<N>& Vector<N>::operator += (const Vector<N>& a)
{
    for (unsigned int i = 0; i < N; ++i)
        _args[i] += a._args[i];
    return *this;
}

template <std::size_t N>
Vector<N>& Vector<N>::operator -= (const Vector<N>& a)
{
    for (size_t i = 0; i < N; ++i)
        _args[i] -= a._args[i];
    return *this;

}

template <std::size_t N>
Vector<N>& Vector<N>::operator *= (const Vector<N>& a)
{
    for (unsigned i = 0; i < N; ++i)
        _args[i] *= a._args[i];
    return *this;

}

template <std::size_t N>
Vector<N>& Vector<N>::operator *= (double factor)
{
    for (unsigned i = 0; i < N; ++i)
        _args[i] *= factor;
    return *this;

}


template <std::size_t N>
Vector<N> operator+ (const Vector<N>& a, const Vector<N>& b)
{
    Vector<N> res(a);
    for (unsigned int i = 0; i < N; ++i)
        res.setArg (i, a.arg (i) + b.arg (i));
    return std::move (res);
}

template <std::size_t N>
Vector<N> operator- (const Vector<N>& a, const Vector<N>& b)
{
    Vector<N> res(a);
    for (unsigned int i = 0; i < N; ++i)
        res.setArg (i, a.arg (i) - b.arg (i));
    return std::move (res);
}
template <std::size_t N>
Vector<N> operator* (const Vector<N>& a, const Vector<N>& b)//分量各自相乘，不是内积也不是外积
{
    Vector<N> res(a);
    for (unsigned int i = 0; i < N; ++i)
        res.setArg (i, a.arg (i) * b.arg (i));
    return std::move (res);
}
template <std::size_t N>
Vector<N> operator* (const Vector<N>& a, double factor)
{
    Vector<N> res(a);
    for (unsigned int i = 0; i < N; ++i)
        res.setArg (i, a.arg (i) * factor);
    return std::move (res);
}
template <std::size_t N>
inline Vector<N> operator* (double factor, const Vector<N>& a)
{
    return std::move (a * factor);
}

template <std::size_t N>
bool operator== (const Vector<N>& a, const Vector<N> &b)
{
    for (unsigned int i =0; i < N; ++i)
        if (fabs(a.arg(i) - b.arg(i)) > EPS)
            return false;
    return true;
}

template <std::size_t N>
Vector<N> standardize (const Vector<N> &a)
{
    double s = 0.0;
    for (unsigned int i = 0; i < N; ++i)
        s += (a.arg (i) * a.arg (i));
    if (fabs(s - 1) < EPS || s <= 0.0 )
        return a;
    s = sqrt (s);
    Vector<N> res(a);
    for (unsigned int i = 0; i < N; ++i)
        res.setArg (i, res.arg (i) / s);
    return std::move(res);
}

template <std::size_t N>
inline double distance2 (const Vector<N>& a, const Vector<N>& b)
{
    double res = 0.0;
    for (unsigned int i = 0; i < N; ++i)
        res += ((a.arg (i) - b.arg (i)) * (a.arg(i) - b.arg (i)));
    return res;
}

template <std::size_t N>
inline double distance (const Vector<N> &a, const Vector<N> &b)
{
    return std::sqrt (distance2 (a, b));
}

template<std::size_t N>
inline double model (const Vector<N> &a)
{
    return distance (a, Vector<N> ());
}

template <std::size_t N>
inline double model2 (const Vector<N> &a)
{
    return distance2 (a, Vector<N> ());
}

//only 3d
inline Vector<3> cross (const Vector<3>& a, const Vector<3>& b)
{
    return Vector<3>(std::array<double, 3>{{a.arg(1) * b.arg(2)-a.arg(2) * b.arg(1), a.arg(2) * b.arg(0) - a.arg(0) * b.arg(2), a.arg(0) * b.arg(1) - a.arg(1) * b.arg(0)}});
}

Vector<3> vertical (const Vector<3>& v);

inline double det (const Vector<3> &a, const Vector<3> &b, const Vector<3> &c)
{
    return a.arg (0) * (b.arg (1) * c.arg (2) - b.arg (2) * c.arg (1)) + b.arg (0) * (c.arg (1) * a.arg (2) - c.arg (2) * a.arg (1)) + c.arg (0) * (a.arg (1) * b.arg (2) - a.arg (2) * b.arg (1));
}

inline Vec3 rotate (const Vec3& vec, const Vec3& r)
{
    double c0 = cos(r.arg (0)), s0 = sin (r.arg (0));
    double c1 = cos(r.arg (1)), s1 = sin (r.arg (1));
    double c2 = cos(r.arg (2)), s2 = sin (r.arg (2));
//    Vec3 res(std::array<double, 3> {{c1 * c2 * vec.arg (0) + (s0 * s1 * c2 - c0 * c1 * s2 ) * vec.arg (1) + (c0 * c2 * s1 + c1 * s0 * s2) * vec.arg (2), c1 * s2 * vec.arg (0) + (c0 * c1 * c2 + s0 * s1 * s2) * vec.arg (1) + (-s0 * c1 * c1 + c0 * s1 * s2) * vec.arg (2), -s1 * vec.arg (0) + c1 * s0 * vec.arg (1) + c0 * c1 * vec.arg (2)}});
    Vec3 res(std::array<double, 3> {{c1 * c2 * vec.arg (0) - (s0 * s1 * c2 + c0 * s2 ) * vec.arg (1) + (s0 * s2 - c0 * c2 *s1) * vec.arg (2), c1 * s2 * vec.arg (0) + (c0 * c2 - s0 * s1 * s2) * vec.arg (1) - (s0 * c2 + c0 * s1 * s2) * vec.arg (2), s1 * vec.arg (0) + c1 * s0 * vec.arg (1) + c0 * c1 * vec.arg (2)}});
    return res;
}
extern std::mt19937 rd;
extern std::uniform_real_distribution<> rand01;
template <size_t N>
inline Vector<N> randomVector ()
{
    Vector<N> res;
    do
    {
        for (size_t i = 0; i < N; ++i)
            res[i] = rand01 (rd) * 2 - 1;
    }
    while (model2 (res) > 1.0);
    return std::move (res);
}

#endif
