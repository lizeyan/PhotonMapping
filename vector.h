#ifndef VECTOR_H
#define VECTOR_H
#include "base.h"
#include <array>
#include <utility>
template <std::size_t N>
class Vector
{
public:
    Vector (const std::array<float, N>& = std::array<float, N>());
    float arg (unsigned int n) const;
    float operator[] (int n) {return arg(n);}
    float operator[] (int n) const {return arg(n);}
    void setArg (int n, float newValue);
    Vector<N>& operator += (const Vector<N>& a);
    Vector<N>& operator -= (const Vector<N>& a);
    Vector<N>& operator *= (const Vector<N>& a);
    Vector<N>& operator *= (float factor);
private:
    std::array<float, N> _args;
};
template <std::size_t N>
Vector<N>::Vector (const std::array<float, N> &args):_args(args)
{
}
template <std::size_t N>
float Vector<N>::arg (unsigned int n) const
{
    if (n < 0 || n >= N)
    {
        return 1e10;
    }
    return _args[n];
}
template <std::size_t N>
void Vector<N>::setArg (int n, float newValue)
{
    if (n < 0 || n >= N)
        return;
    _args[n] = newValue;
}

template <std::size_t N>
float dot (const Vector<N>& a, const Vector<N>& b)
{
    float res = 0;
    for (int i = 0; i < N; ++i)
        res += (a.arg (i) * b.arg (i));
    return res;
}
template <std::size_t N>
Vector<N>& Vector<N>::operator += (const Vector<N>& a)
{
    for (int i = 0; i < N; ++i)
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
    for (int i = 0; i < N; ++i)
        res.setArg (i, a.arg (i) + b.arg (i));
    return std::move (res);
}

template <std::size_t N>
Vector<N> operator- (const Vector<N>& a, const Vector<N>& b)
{
    Vector<N> res(a);
    for (int i = 0; i < N; ++i)
        res.setArg (i, a.arg (i) - b.arg (i));
    return std::move (res);
}
template <std::size_t N>
Vector<N> operator* (const Vector<N>& a, const Vector<N>& b)//分量各自相乘，不是内积也不是外积
{
    Vector<N> res(a);
    for (int i = 0; i < N; ++i)
        res.setArg (i, a.arg (i) * b.arg (i));
    return std::move (res);
}
template <std::size_t N>
Vector<N> operator* (const Vector<N>& a, float factor)
{
    Vector<N> res(a);
    for (int i = 0; i < N; ++i)
        res.setArg (i, a.arg (i) * factor);
    return std::move (res);
}
template <std::size_t N>
Vector<N> operator* (float factor, const Vector<N>& a)
{
    return std::move (a * factor);
}

template <std::size_t N>
Vector<N> normalize (const Vector<N> &a)
{
    float s = 0;
    for (int i = 0; i < N; ++i)
        s += (a.arg (i) * a.arg (i));
    if (s < EPS)
        return a;
    Vector<N> res(a);
    for (int i = 0; i < N; ++i)
        res.setArg (res.arg (i) / res);
    return std::move(res);
}

#endif
