#ifndef VEC3_H
#define VEC3_H

#include <iostream>
#include <cmath>

class Vec3 {
    private:
        double e[3];
    
    public:
        Vec3() : e{0., 0. ,0.} {}
        Vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }

        double operator[](auto i) const { return e[i]; }
        double& operator[](auto i) {return e[i]; }

        Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
        
        Vec3 operator+=(const Vec3& v) {
            e[0] += v.x();
            e[1] += v.y();
            e[2] += v.z();

            return *this;
        }

        Vec3 operator-=(const Vec3& v) {
            e[0] -= v.x();
            e[1] -= v.y();
            e[2] -= v.z();

            return *this;
        }

        Vec3 operator*=(double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;

            return *this;
        }

        Vec3 operator/=(double t) {
            if (t == 0) {
                e[0] = std::nan("");
                e[1] = std::nan("");
                e[2] = std::nan("");

                return *this;
            }

            return *this *= 1/t;
        }

        double length_squared() const {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }

        double length() const {
            return std::sqrt(length_squared());
        }
}; // class Vec3

inline std::ostream& operator<<(std::ostream& out, const Vec3& v) {
    return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

inline Vec3 operator+(const Vec3& u, const Vec3& v) {
    return Vec3(u.x()+v.x(), u.y()+v.y(), u.z()+v.z());
}

inline Vec3 operator-(const Vec3& u, const Vec3& v) {
    return Vec3(u.x()-v.x(), u.y()-v.y(), u.z()-v.z());
}

inline Vec3 operator*(const Vec3& u, const Vec3& v) {
    return Vec3(u.x()*v.x(), u.y()*v.y(), u.z()*v.z());
}

inline Vec3 operator*(const Vec3& v, double t) {
    return Vec3(v.x()*t, v.y()*t, v.z()*t);
}

inline Vec3 operator*(double t, const Vec3& v) {
    return v*t;
}

inline Vec3 operator/(const Vec3& v, double t) {
    if (t == 0) {
        return Vec3(std::nan(""), std::nan(""), std::nan(""));
    }

    return v*(1/t);
}

inline double dot(const Vec3& u, const Vec3& v) {
    return u.x()*v.x() + u.y()*v.y() + u.z()*v.z();
}

inline Vec3 cross(const Vec3& u, const Vec3& v) {
    return Vec3(
        u.y()*v.z() - u.z()*v.y(),
        u.z()*v.x() - u.x()*v.z(),
        u.x()*v.y() - u.y()*v.x());
} 

inline Vec3 unit_vector(const Vec3& v) {
    return v / v.length();
}
#endif