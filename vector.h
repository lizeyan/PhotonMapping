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
    Vector (const std::array<float, N>& = std::array<float, N>());
    float arg (unsigned int n) const;
    float& operator[] (unsigned int n);
    void setArg (int n, float newValue);
    Vector<N>& operator += (const Vector<N>& a);
    Vector<N>& operator -= (const Vector<N>& a);
    Vector<N>& operator *= (const Vector<N>& a);
    Vector<N>& operator *= (float factor);
private:
    std::array<float, N> _args;
};
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
Vector<N>::Vector (const std::array<float, N> &args):_args(args)
{
}
template <std::size_t N>
float Vector<N>::arg (unsigned int n) const
{
    if (n < 0 || n >= N)
    {
        throw std::out_of_range("n of Vector<N>::arg incorrect");
    }
    return _args[n];
}
template <std::size_t N>
float& Vector<N>::operator [] (unsigned int n)
{
    if (n < 0 || n >= N)
    {
        throw std::out_of_range("n of Vector<N>::arg incorrect");
    }
    return _args[n];
}

template <std::size_t N>
void Vector<N>::setArg (int n, float newValue)
{
    if (n < 0 || n >= int(N))
        return;
    _args[n] = newValue;
}

template <std::size_t N>
float dot (const Vector<N>& a, const Vector<N>& b)
{
    float res = 0;
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
    for (int i = 0; i < N; ++i)
        _args[i] -= a._args[i];
    return *this;

}

template <std::size_t N>
Vector<N>& Vector<N>::operator *= (const Vector<N>& a)
{
    for (int i = 0; i < N; ++i)
        _args[i] *= a._args[i];
    return *this;

}

template <std::size_t N>
Vector<N>& Vector<N>::operator *= (float factor)
{
    for (int i = 0; i < N; ++i)
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
Vector<N> operator* (const Vector<N>& a, float factor)
{
    Vector<N> res(a);
    for (unsigned int i = 0; i < N; ++i)
        res.setArg (i, a.arg (i) * factor);
    return std::move (res);
}
template <std::size_t N>
Vector<N> operator* (float factor, const Vector<N>& a)
{
    return std::move (a * factor);
}

template <std::size_t N>
bool operator== (const Vector<N>& a, const Vector<N> &b)
{
    for (unsigned int i =0; i < N; ++i)
        if (a.arg(i) != b.arg(i))
            return false;
    return true;
}

template <std::size_t N>
Vector<N> standardize (const Vector<N> &a)
{
    float s = 0;
    for (unsigned int i = 0; i < N; ++i)
        s += (a.arg (i) * a.arg (i));
    if (fabs(s - 1) < EPS || fabs(s) < EPS)
        return a;
    s = sqrt (s);
    Vector<N> res(a);
    for (unsigned int i = 0; i < N; ++i)
        res.setArg (i, res.arg (i) / s);
    return std::move(res);
}

template <std::size_t N>
float distance (const Vector<N> &a, const Vector<N> &b)
{
    float res = 0;
    for (unsigned int i = 0; i < N; ++i)
        res += ((a.arg (i) - b.arg (i)) * (a.arg(i) - b.arg (i)));
    return res;
}

template<std::size_t N>
float model (const Vector<N> &a)
{
    return distance (a, Vector<N> ());
}

//only 3d
inline Vector<3> cross (const Vector<3>& a, const Vector<3>& b)
{
    return Vector<3>(std::array<float, 3>{{a.arg(1) * b.arg(2)-a.arg(2) * b.arg(1), a.arg(2) * b.arg(0) - a.arg(0) * b.arg(2), a.arg(0) * b.arg(1) - a.arg(1) * b.arg(0)}});
}

Vector<3> vertical (const Vector<3>& v, const Vector<3> &p);

inline float det (const Vector<3> &a, const Vector<3> &b, const Vector<3> &c)
{
    return a.arg (0) * (b.arg (1) * c.arg (2) - b.arg (2) * c.arg (1)) + b.arg (0) * (c.arg (1) * a.arg (2) - c.arg (2) * a.arg (1)) + c.arg (0) * (a.arg (1) * b.arg (2) - a.arg (2) * b.arg (1));
}
#endif
