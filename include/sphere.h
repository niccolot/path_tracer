#ifndef SPHERE_H
#define SPHERE_H

#include <memory>

#include "hittable.h"
#include "vec3.h"

class Sphere : public Hittable {
    private:
        Vec3 center;
        double radius;
        std::shared_ptr<Material> mat;
    
    public:
        Sphere() = delete;
        Sphere(const Vec3& center, double radius) : center(center), radius(radius) {}

        virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;
}; // class Sphere
#endif