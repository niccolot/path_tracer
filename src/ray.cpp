#include "ray.h"
#include "hittable.h"

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
