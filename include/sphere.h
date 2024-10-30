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
        static void get_sphere_uv(const Vec3& p, double& u, double& v);
    
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
        double pdf_value(
            [[maybe_unused]] const Vec3& origin, 
            [[maybe_unused]] const Vec3& direction) const override{ return 0; }
        
        Vec3 random(
            [[maybe_unused]] const Vec3& origin) const override { return Vec3(1,0,0); }
}; // class Sphere
#endif