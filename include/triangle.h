#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <cassert>

#include "planar.h"
#include "quad.h"
#include "material.h"


class Triangle : public Planar {
    private:
        bool is_interior(double a, double b, HitRecord &rec) const override;

    public:
        Triangle(
            const Vec3& Q,
            const Vec3& u,
            const Vec3& v,
            std::shared_ptr<Material> mat) : Planar(Q,u,v,mat) { set_bounding_box(); }

        void set_bounding_box() override { bbox = AxisAlignedBBox(_Q + _u, _Q + _v); };

        Vec3 random(const Vec3& origin) const override; 
}; // class Triangle

template <class BaseType>
std::shared_ptr<HittableList> pyramid(const BaseType& base, double h, std::shared_ptr<Material> mat) {
    /**
     * @brief given a Quad or Triangle sphape as base,
     * return a pyramid object assembling the sides inside
     * an HittableList.
     * 
     * @details if the pyramid appears upside down, invert the 
     * u,v vectors of the base, this will invert the direction
     * of the base normal vector
     */
    auto sides = std::make_shared<HittableList>();

    auto Q = base.Q();
    auto u = base.u();
    auto v = base.v();

    std::vector<Vec3> base_points = {Q, Q+u, Q+v};
    Vec3 base_centroid;
    Vec3 vertex;

    sides->add(std::make_shared<BaseType>(base));

    if constexpr (std::is_same_v<BaseType, Quad>) {
        base_points.push_back(Q+u+v);

        base_centroid = mean(base_points);
        vertex = base_centroid + unit_vector(base.normal()) * h;

        auto vertex_Q = vertex - Q;
        auto vertex_Q_u = vertex_Q - u;
        auto vertex_Q_v = vertex_Q - v;
        auto vertex_Q_u_v = vertex_Q_u - v;

        sides->add(std::make_shared<Triangle>(Q, u, vertex_Q, mat));
        sides->add(std::make_shared<Triangle>(Q+u, v, vertex_Q_u, mat));
        sides->add(std::make_shared<Triangle>(Q+u+v, -u, vertex_Q_u_v, mat));
        sides->add(std::make_shared<Triangle>(Q+v, -v, vertex_Q_v, mat));
    } else {
        base_centroid = mean(base_points);
        vertex = base_centroid + unit_vector(base.normal()) * h;

        auto vertex_Q = vertex - Q;
        auto vertex_Q_u = vertex_Q - u;
        auto vertex_Q_v = vertex_Q - v;

        sides->add(std::make_shared<Triangle>(Q, u, vertex_Q, mat));
        sides->add(std::make_shared<Triangle>(Q+u, v-u, vertex_Q_u, mat));
        sides->add(std::make_shared<Triangle>(Q+v, -v, vertex_Q_v, mat));
    }

    return sides;
}

std::shared_ptr<HittableList> quad_pyramid(const Quad& base, double h, std::shared_ptr<Material> mat);
std::shared_ptr<HittableList> triangle_pyramid(const Triangle& base, double h, std::shared_ptr<Material> mat);
#endif
