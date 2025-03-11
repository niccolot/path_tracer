#ifndef VEC3_H
#define VEC3_H

#include <iostream>
#include <limits>

template<typename T>
class Vec3 {

private:
    T e[3];

public:
    Vec3() : e{0, 0, 0} {}
    Vec3(T x, T y, T z) : e{x, y, z} {}
    
    Vec3(const Vec3<T>&) = default;

    T x() const { return e[0]; }
    T y() const { return e[1]; }
    T z() const { return e[2]; }

    void set_x(T x) { e[0] = x; }
    void set_y(T y) { e[1] = y; }
    void set_z(T z) { e[2] = z; }

    T operator[](uint32_t i) const { return e[i]; }
    T& operator[](uint32_t i) { return e[i]; }

    Vec3<T> operator=(const Vec3<T>& v) {
        e[0] = v.x();
        e[1] = v.y();
        e[2] = v.z();

        return *this;
    }

    Vec3<T> operator-() const { return Vec3<T>(-e[0], -e[1], -e[1]); }
    
    Vec3<T> operator+=(const Vec3<T>& v) {
        e[0] += v.x();
        e[1] += v.y();
        e[2] += v.z();

        return *this;
    }

    Vec3<T> operator-=(const Vec3<T>& v) {
        return *this += -v;
    }

    Vec3<T> operator*=(T t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;

        return *this;
    } 

    Vec3<T> operator/=(T t) {
        return *this *= 1/t;
    }

    T length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    T length() const {
        return std::sqrt(length_squared());
    }
}; // class Vec3

template<typename T>
inline std::ostream& operator<<(std::ostream& out, const Vec3<T>& v) {
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

template<typename T>
inline Vec3<T> operator+(const Vec3<T>& u, const Vec3<T>& v) {
    return Vec3<T>(u.x() + v.x(), u.y() + v.y(), u.z() + v.z());
}

template<typename T>
inline Vec3<T> operator-(const Vec3<T>& u, const Vec3<T>& v) {
    return u + (-v);
}

template<typename T>
inline Vec3<T> operator*(const Vec3<T>& v,T t) {
    return Vec3<T>(v.x() * t, v.y() * t, v.z() * t);
}

template<typename T>
inline Vec3<T> operator*(T t, const Vec3<T>& v) {
    return v * t;
}

template<typename T>
inline Vec3<T> operator/(const Vec3<T>& v,T t) {
    return v * (1/t);
}

template<typename T>
inline Vec3<T> operator/(T t, const Vec3<T>& v) {
    return v / t;
}

template<typename T>
inline T dot(const Vec3<T> u, const Vec3<T>& v) {
    return u.x() * v.x() + u.y() * v.y() + u.z() * v.z();
}

template<typename T>
inline Vec3<T> cross(const Vec3<T> u, const Vec3<T>& v) {
    return Vec3<T>(
        u.y()*v.z() - u.z()*v.y(),
        u.z()*v.x() - u.x()*v.z(),
        u.x()*v.y() - u.y()*v.x());
}

template<typename T>
inline Vec3<T> unit_vector(const Vec3<T>& v) {
    return v / v.length();
}

using Vec3f = Vec3<float>;
using Vec3Double = Vec3<double>;
#endif