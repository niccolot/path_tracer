#ifndef SPHERE_H
#define SPHERE_H

#include <memory>

#include "hittable.h"
#include "vec3.h"

class Sphere : public Hittable {
    private:
        Ray center;
        double radius;
        std::shared_ptr<Material> mat;
    
    public:
        Sphere() = delete;

        // static sphere
        Sphere(const Vec3& static_center, double radius, std::shared_ptr<Material> mat) 
        : center(static_center, Vec3()), radius(radius), mat(mat) {}

        // moving sphere
        Sphere(
            const Vec3& center1,
            const Vec3& center2, 
            double radius, 
            std::shared_ptr<Material> mat) :

                center(center1, center2 - center1),
                radius(radius),
                mat(mat) {}

        virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;
}; // class Sphere
#endif