#ifndef QUAD_H
#define QUAD_H

#include <memory>

#include "hittable.h"

class Quad : public Hittable {
    private:
        Vec3 Q; // origin point
        Vec3 u, v; // vectors spanning 2 edges
        std::shared_ptr<Material> mat;
        AxisAlignedBBox bbox;
        Vec3 normal;
        double D; // plane's distance from origin
        Vec3 w;
        double area;
        virtual bool is_interior(double a, double b, HitRecord& rec) const;
    

    public:
        Quad(
            const Vec3& Q,
            const Vec3& u,
            const Vec3& v,
            std::shared_ptr<Material> mat);

        const AxisAlignedBBox& bounding_box() const { return bbox; }

        virtual void set_bounding_box();
        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;
        double pdf_value(const Vec3& origin, const Vec3& direction) const override;
        Vec3 random(const Vec3& origin) const override;
}; // class Quad

inline std::shared_ptr<HittableList> box(const Vec3& a, const Vec3& b, std::shared_ptr<Material> mat) {
    /**
     * @brief returns a box with 'a' and 'b' as opposite vertices
     */

    auto sides = std::make_shared<HittableList>();

    auto min_x = std::fmin(a.x(), b.x());
    auto min_y = std::fmin(a.y(), b.y());
    auto min_z = std::fmin(a.z(), b.z());
    
    auto max_x = std::fmax(a.x(), b.x());
    auto max_y = std::fmax(a.y(), b.y());
    auto max_z = std::fmax(a.z(), b.z());

    auto dx = Vec3(max_x - min_x, 0, 0);
    auto dy = Vec3(0, max_y - min_y, 0);
    auto dz = Vec3(0, 0, max_z - min_z);

    sides->add(std::make_shared<Quad>(Vec3(min_x, min_y, max_z), dx, dy, mat)); // front
    sides->add(std::make_shared<Quad>(Vec3(max_x, min_y, max_z), -dz, dy, mat)); // right
    sides->add(std::make_shared<Quad>(Vec3(max_x, min_y, min_z), -dx, dy, mat)); // back
    sides->add(std::make_shared<Quad>(Vec3(min_x, min_y, min_z), dz, dy, mat)); // left
    sides->add(std::make_shared<Quad>(Vec3(min_x, max_y, max_z), dx, -dz, mat)); // top
    sides->add(std::make_shared<Quad>(Vec3(min_x, min_y, min_z), dx, dz, mat)); // bottom

    return sides;
}
#endif