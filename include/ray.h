#ifndef RAY_H
#define RAY_H

#include "vec3.h"
#include "constants.h"


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
#endif