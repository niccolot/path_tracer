#ifndef RAY_H
#define RAY_H

#include "vec3.h"
#include "constants.h"


class Ray {
    private:
        Vec3 orig;
        Vec3 dir;
        double time_val;
    
    public:
        Ray(const Vec3& o, const Vec3& d, double time) : 
            orig(o), 
            dir(d),
            time_val(time) {}

        Ray(const Vec3& o, const Vec3& d) : 
            Ray(o, d, 0) {}
        
        Ray() : Ray(Vec3(), Vec3(), 0) {}

        const Vec3& origin() const { return orig; }
        const Vec3& direction() const { return dir; }
        double time() const { return time_val; }

        const Vec3 at(double t) const { return orig + t*dir; }

        double hit_sphere(const Vec3& center, double radius) const;
}; // class Ray
#endif