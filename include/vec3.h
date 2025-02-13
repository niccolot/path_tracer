#ifndef VEC3_H
#define VEC3_H

#include <iostream>
#include <cmath>

#include "utils.h"

class Vec3 {
    private:
        double e[3];

    public:
        Vec3() : e{0., 0. ,0.} {}
        Vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }

        void set_x(double x) { e[0] = x; }
        void set_y(double y) { e[1] = y; }
        void set_z(double z) { e[2] = z; }

        double operator[](auto i) const { return e[i]; }
        double& operator[](auto i) {return e[i]; }

        Vec3 operator=(const Vec3& v) {
            e[0] = v.x();
            e[1] = v.y();
            e[2] = v.z();

            return *this;
        }

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

        static Vec3 random() {
            return Vec3(random_double(), random_double(), random_double());
        }
        
        static Vec3 random(double min, double max) {
            return Vec3(random_double(min, max), random_double(min, max), random_double(min, max));
        }

        double length_squared() const {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }

        double length() const {
            return std::sqrt(length_squared());
        }

        bool near_zero() const {
            auto tol = 1e-8;
            auto x_comp = std::fabs(e[0]) < tol;
            auto y_comp = std::fabs(e[1]) < tol;
            auto z_comp = std::fabs(e[2]) < tol; 
            
            return x_comp && y_comp && z_comp;;
        }

        bool is_nan() const {
            return std::isnan(e[0]) || std::isnan(e[1]) || std::isnan(e[2]);
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

inline Vec3 sample_square() {
    /**
     * @brief returns a Vec3 randomly sampled in the [-0.5, 0.5] X [-0.5, 0.5]
     * unit square
     */

    return Vec3(random_double(-0.5, 0.5), random_double(-0.5, 0.5), 0);
}

inline Vec3 random_unit_vector() {
    /**
     * @brief generates random vectors inside the unit square 
     * containing the unit circle. Once a generated vector is
     * inside the circle and not too short for numerical overlow,
     * it is returned
     */
    while (true) {
        auto p = Vec3::random(-1,1);
        auto lensq = p.length_squared();
        if (lensq > 1e-160 && lensq <= 1) {
            return p / std::sqrt(lensq);
        }
    }
}

inline Vec3 random_in_unit_disk() {
    while (true) {
        auto p = Vec3(random_double(-1,1), random_double(-1, 1), 0);
        if (p.length_squared() < 1) {
            return p;
        }
    }
}

inline Vec3 random_cosine_direction() {
    auto r1 = random_double();
    auto r2 = random_double();

    auto phi = 2*pi*r2;
    auto x = std::cos(phi) * std::sqrt(1 - r1);
    auto y = std::sin(phi) * std::sqrt(1 - r1);
    auto z = std::sqrt(r1);

    return Vec3(x,y,z);
}

inline Vec3 random_phong_direction(int n) {
    /**
     * @param n phong specular exponent
     */
    auto r1 = random_double();
    auto r2 = random_double();

    auto phi = 2*pi*r2;
    
    auto r1_1 = std::pow(r1, 2./(1+n));
    auto r1_2 = std::pow(r1, 1./(1+n));

    auto x = std::cos(phi)*std::sqrt(1-r1_1);
    auto y = std::sin(phi)*std::sqrt(1-r1_1);
    auto z = r1_2;
    
    return Vec3(x,y,z);
}

inline Vec3 reflect(const Vec3& v, const Vec3& n) {
    /**
     * @brief returns the perfectly reflected vector
     * for incidence on a surface with normal n
     * 
     * @param v incident vector. The direction is intended
     * to be towards the reflecting surface, i.e. antiparallel
     * with the surface normal
     * @param n surface normal vector, assumed to be unitary
     */

    return v - 2*dot(v,n)*n;
}

inline Vec3 refract(const Vec3& v, const Vec3& n, double etaI_over_etaT) {
    /**
     * @brief return the refracted vector according to snell law
     * 
     * @param n surface normal, assumed to be unitary
     * @param etaI_over_etaT ration between refractive index
     * for the incident vector and refractive index for the
     * transmitted vector
     */

    auto cos_theta = std::fmin(dot(-v,n), 1.);
    Vec3 r_out_perp = etaI_over_etaT * (v + cos_theta*n);
    Vec3 r_out_par = -std::sqrt(std::fabs(1. - r_out_perp.length_squared())) * n;

    return r_out_perp + r_out_par;
}

inline Vec3 mean(const std::vector<Vec3>& vectors) {
    /**
     * @brief returns a Vec3 with each component
     * equal to the mean of each input vector component
     */
    auto mean = Vec3();

    for (const auto& vec : vectors) {
        mean.set_x(mean.x() + vec.x());
        mean.set_y(mean.y() + vec.y());
        mean.set_z(mean.z() + vec.z());
    }

    return mean / vectors.size();
}
#endif