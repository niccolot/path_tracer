#ifndef QUAD_H
#define QUAD_H

#include "planar.h"

class Quad : public Planar {
    private:
        bool is_interior(double a, double b, HitRecord &rec) const override;

    public:
    /**
     * @details if one wants e.g. a square with bottom left corner
     * in (0,0,-1) with edges = 1 the initial parameters are
     * Q = Vec3(0,0,-1), u = Vec3(1,0,0), v = Vec3(0,1,0)
     */
    Quad(
        const Vec3& Q,
        const Vec3& u,
        const Vec3& v,
        std::shared_ptr<Material> mat) : Planar(Q,u,v,mat) { set_bounding_box(); }

        void set_bounding_box() override;
        Vec3 random(const Vec3& origin) const override;
}; // class Quad

std::shared_ptr<HittableList> box(const Vec3& a, const Vec3& b, std::shared_ptr<Material> mat);
#endif