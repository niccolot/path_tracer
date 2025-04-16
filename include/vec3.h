#ifndef VEC3_H
#define VEC3_H

#include <iostream>
#include <cmath>
#include <limits>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <OBJ_Loader.h>
#pragma GCC diagnostic pop

#include "random.h"

class Vec3f {
private:
    float _e[3];

public:
    Vec3f() : _e{0.f, 0.f, 0.f} {}
    Vec3f(float x, float y, float z) : _e{x, y, z} {}
    Vec3f(const Vec3f&) = default;
    Vec3f(const objl::Vector3& v) : _e{v.X, v.Y, v.Z} {}

    float x() const { return _e[0]; }
    float y() const { return _e[1]; }
    float z() const { return _e[2]; }

    void set_x(float x) { _e[0] = x; }
    void set_y(float y) { _e[1] = y; }
    void set_z(float z) { _e[2] = z; }

    float operator[](uint32_t i) const { return _e[i]; }
    float& operator[](uint32_t i) { return _e[i]; }

    Vec3f operator=(const Vec3f& v) {
        _e[0] = v.x();
        _e[1] = v.y();
        _e[2] = v.z();

        return *this;
    }

    Vec3f operator-() const { return Vec3f(-_e[0], -_e[1], -_e[2]); }

    Vec3f operator+=(const Vec3f& v) {
        _e[0] += v.x();
        _e[1] += v.y();
        _e[2] += v.z();

        return *this;
    }

    Vec3f operator-=(const Vec3f& v) {
        return *this += -v;
    }

    Vec3f operator*=(float t) {
        _e[0] *= t;
        _e[1] *= t;
        _e[2] *= t;

        return *this;
    } 

    Vec3f operator/=(float t) {
        return *this *= 1/t;
    }

    float length_squared() const {
        return _e[0] * _e[0] + _e[1] * _e[1] + _e[2] * _e[2];
    }

    float length() const {
        return std::sqrt(length_squared());
    }
}; // class Vec3f

inline std::ostream& operator<<(std::ostream& out, const Vec3f& v) {
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

inline Vec3f operator+(const Vec3f& u, const Vec3f& v) {
    return Vec3f(u.x() + v.x(), u.y() + v.y(), u.z() + v.z());
}

inline Vec3f operator-(const Vec3f& u, const Vec3f& v) {
    return u + (-v);
}

inline Vec3f operator*(const Vec3f& v, float t) {
    return Vec3f(v.x() * t, v.y() * t, v.z() * t);
}

inline Vec3f operator*(float t, const Vec3f& v) {
    return v * t;
}

inline Vec3f operator/(const Vec3f& v, float t) {
    return v * (1/t);
}

inline Vec3f operator/(float t, const Vec3f& v) {
    return Vec3f(t / v.x(), t / v.y(), t / v.z());
}

inline float dot(const Vec3f& u, const Vec3f& v) {
    return u.x() * v.x() + u.y() * v.y() + u.z() * v.z();
}

inline Vec3f cross(const Vec3f& u, const Vec3f& v) {
    return Vec3f(
        u.y()*v.z() - u.z()*v.y(),
        u.z()*v.x() - u.x()*v.z(),
        u.x()*v.y() - u.y()*v.x());
}

inline Vec3f unit_vector(const Vec3f& v) {
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