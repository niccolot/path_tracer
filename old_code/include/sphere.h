#ifndef SPHERE_H
#define SPHERE_H

#include <memory>

#include "hittable.h"
#include "vec3.h"
#include "onb.h"

class Sphere : public Hittable {
    private:
        Ray center;
        double radius;
        std::shared_ptr<Material> mat;
        AxisAlignedBBox bbox;
        static void get_sphere_uv(const Vec3& p, double& u, double& v);
        static Vec3 random_to_sphere(double radius, double distance_squared);
    
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
        double pdf_value(const Vec3& origin, const Vec3& direction) const override; 
        Vec3 random(const Vec3& origin) const override; 
}; // class Sphere
#endif