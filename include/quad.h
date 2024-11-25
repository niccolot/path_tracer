#ifndef QUAD_H
#define QUAD_H

#include "planar.h"

class Quad : public Planar {
    private:
        bool is_interior(double a, double b, HitRecord &rec) const override;

    public:
        Quad(
            const Vec3& Q,
            const Vec3& u,
            const Vec3& v,
            std::shared_ptr<Material> mat,
            bool verts = false);

        void set_bounding_box() override;
        Vec3 random(const Vec3& origin) const override;
        //const std::vector<std::shared_ptr<Hittable>>& get_objects() {};
}; // class Quad

std::shared_ptr<HittableList> box(const Vec3& a, const Vec3& b, std::shared_ptr<Material> mat);
#endif