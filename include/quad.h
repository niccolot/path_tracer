#ifndef QUAD_H
#define QUAD_H

#include <memory>

#include "hittable.h"

class Quad : public Hittable {
    private:
        Vec3 Q; // one of the vertices
        Vec3 u, v; // edges from Q to other vertex
        std::shared_ptr<Material> mat;
        AxisAlignedBBox bbox;
        Vec3 normal;
        double D; // plane's distance from origin
        Vec3 w;
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

}; // class Quad
#endif