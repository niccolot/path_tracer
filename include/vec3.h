#ifndef VEC3_H
#define VEC3_H

#include <iostream>
#include <cmath>
#include <limits>

#include "random.h"

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

    Vec3<T> operator-() const { return Vec3<T>(-e[0], -e[1], -e[2]); }
    
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

using Vec3f = Vec3<float>;
using Vec3Double = Vec3<double>;

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

template<typename T, typename U>
inline Vec3<T> operator*(const Vec3<T>& v, U t) {
    return Vec3<T>(v.x() * t, v.y() * t, v.z() * t);
}

template<typename T, typename U>
inline Vec3<T> operator*(U t, const Vec3<T>& v) {
    return v * t;
}

template<typename T, typename U>
inline Vec3<T> operator/(const Vec3<T>& v, U t) {
    return v * (1/t);
}

template<typename T, typename U>
inline Vec3<T> operator/(T t, const Vec3<T>& v) {
    return v / t;
}

template<typename T>
inline T dot(const Vec3<T>& u, const Vec3<T>& v) {
    return u.x() * v.x() + u.y() * v.y() + u.z() * v.z();
}

template<typename T>
inline Vec3<T> cross(const Vec3<T>& u, const Vec3<T>& v) {
    return Vec3<T>(
        u.y()*v.z() - u.z()*v.y(),
        u.z()*v.x() - u.x()*v.z(),
        u.x()*v.y() - u.y()*v.x());
}

template<typename T>
inline Vec3<T> unit_vector(const Vec3<T>& v) {
    return v / v.length();
}

inline Vec3f random_vector() {
    return Vec3f(RandomUtils::random_float(), RandomUtils::random_float(), RandomUtils::random_float());
}

inline Vec3f random_vector(float min, float max) {
    return Vec3f(RandomUtils::random_float(min, max), RandomUtils::random_float(min, max), RandomUtils::random_float(min, max));
}

inline Vec3f random_unit_vector() {
    /**
     * @brief: generates random vectors inside the unit square 
     * containing the unit circle. Once a generated vector is
     * inside the circle and not too short for numerical overlow,
     * it is returned
     */
    while(true) {
        Vec3f p = random_vector(-1.f, 1.f);
        float lensq = p.length_squared();
        if (lensq > 1e-80 && lensq <= 1.f) {
            return p / std::sqrt(lensq);
        }
    }
}

inline Vec3f rotate_spherically(const Vec3f& v1, const Vec3f& v2, float theta, float phi) {
    /**
     * @brief: rotates a vector by angles (theta, phi) in spherical coordinates
     * @details: by choice theta refers to the angle between the y axis (vertical by default)
     * and the XZ plane
     * @param r: radius of vector to rotate
     */

    float r = (v1 - v2).length();
    float x = r * std::sin(theta) * std::sin(phi);
    float y = r * std::cos(theta);
    float z = r * std::sin(theta) * std::cos(phi);

    Vec3f v = v2;
    v.set_x(v.x() + x);
    v.set_y(v.y() + y);
    v.set_z(v.z() + z);

    return v;
}
#endif