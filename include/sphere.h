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
        AxisAlignedBBox bbox;
    
    public:
        Sphere() = delete;

        // static sphere
        Sphere(
            const Vec3& static_center, 
            double radius, 
            std::shared_ptr<Material> mat);

        // moving sphere
        Sphere(
            const Vec3& center1,
            const Vec3& center2, 
            double radius, 
            std::shared_ptr<Material> mat);

        virtual const AxisAlignedBBox& bounding_box() const override { return bbox; }
        virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;
}; // class Sphere
#endif