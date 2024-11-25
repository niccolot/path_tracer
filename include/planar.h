#ifndef PLANAR_H
#define PLANAR_H

#include <vector>

#include "hittable.h"

class Planar : public Hittable {
    protected:
        Vec3 _Q; // origin point
        Vec3 _u, _v; // vectors spanning the planar figure
        std::shared_ptr<Material> _mat;
        AxisAlignedBBox bbox;
        Vec3 _normal;
        double D; // figure's plane distance from origin
        Vec3 w;
        std::vector<Vec3> vertices;
        double area;
        void muller_trumbore_intersect(double& alpha, double& beta, const Vec3& planar_hitpt_vec) const;
        virtual bool is_interior(double a, double b, HitRecord& rec) const = 0;

    public:
        Planar(
            const Vec3& Q,
            const Vec3& u,
            const Vec3& v,
            std::shared_ptr<Material> mat,
            bool verts = false);

        const Vec3& Q() const { return _Q; }
        const Vec3& u() const { return _u; }
        const Vec3& v() const { return _v; }
        const Vec3& normal() const { return _normal; }

        const AxisAlignedBBox& bounding_box() const override { return bbox; }
        virtual void set_bounding_box() = 0;
        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const;
        double pdf_value(const Vec3& origin, const Vec3& direction) const;
        virtual Vec3 random(const Vec3& origin) const = 0;
        //virtual const std::vector<std::shared_ptr<Hittable>>& get_objects() const = 0;
}; // class Planar
#endif