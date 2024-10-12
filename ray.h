#ifndef RAY_H
#define RAY_H

#include "vec3.h"
#include "color.h"

class Ray {
    private:
        Vec3 orig;
        Vec3 dir;
    
    public:
        Ray() : orig(Vec3()), dir(Vec3()) {}
        Ray(const Vec3& o, const Vec3& d) : orig(o), dir(d) {}

        const Vec3& origin() const { return orig; }
        const Vec3& direction() const { return dir; }

        const Vec3 at(double t) const { return orig + t*dir; }

        double hit_sphere(const Vec3& center, double radius) const;
}; // class Ray

double Ray::hit_sphere(const Vec3& center, double radius) const {
    Vec3 oc = center - orig;
    auto a = dot(dir, dir);
    auto b = -2.0*dot(dir, oc);
    auto c = dot(oc, oc) - radius*radius;
    auto discriminant = b*b - 4*a*c;
    if (discriminant < 0) {
        return -1.0;
    } else {
        return ((-b - std::sqrt(discriminant)) / (2.*a));
    }
}

Color ray_color(const Ray& r) {
    auto t = r.hit_sphere(Vec3(0,0,-1), .5);
    if (t > 0.) {
        Vec3 n = unit_vector(r.at(t) - Vec3(0,0,-1));
        return 0.5*Color(n.x()+1, n.y()+1, n.z()+1);
    }

    Vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.); // |a| in [0, 1]

    return (1.0-a)*Color(1., 1., 1.) + a*Color(0.5, 0.7, 1.0);
}
#endif