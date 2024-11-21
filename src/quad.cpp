#include "quad.h"

Quad::Quad(
    const Vec3& Q,
    const Vec3& u,
    const Vec3& v,
    std::shared_ptr<Material> mat,
    bool verts) : Planar(Q,u,v,mat,verts) {
    /**
     * @details if one wants e.g. a square with bottom left corner
     * in (0,0,-1) with edges = 1 the initial parameters are
     * Q = Vec3(0,0,-1) 
     * u = Vec3(1,0,0) 
     * v = Vec3(0,1,0)
     * with verts flag false, 
     * 
     * Q = Vec3(0,0,-1)
     * u = Vec3(1,0,-1)
     * v = Vec3(0,1,-1)
     * with verts flag true
     */
        
    area = _normal.length();
    set_bounding_box();
    if (!verts) {
        vertices.emplace_back(Q+u+v);
    } else {
        vertices.emplace_back(u+v);
    }
     
}

void Quad::set_bounding_box() {
    auto bbox_diagonal1 = AxisAlignedBBox(_Q, _Q + _u + _v);
    auto bbox_diagonal2 = AxisAlignedBBox(_Q + _u, _Q + _v);
    bbox = AxisAlignedBBox(bbox_diagonal1, bbox_diagonal2);
}

bool Quad::is_interior(double a, double b, HitRecord& rec) const {
    /**
     * @brief given an hit on the quadrilateral plane
     * returns true if the hit is inside the acutal quad
     */
    Interval unit_interval = Interval(0,1);
    if (!unit_interval.contains(a) || !unit_interval.contains(b)) {
        return false;
    }

    rec.set_u(a);
    rec.set_v(b);

    return true;
}

Vec3 Quad::random(const Vec3& origin) const {
    auto p = Q() + (random_double() * u()) + (random_double() * v());

    return p - origin;
}

std::shared_ptr<HittableList> box(const Vec3& a, const Vec3& b, std::shared_ptr<Material> mat) {
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

    sides->add(std::make_shared<Quad>(Vec3(min_x, min_y, max_z), dx, dy, mat)); 
    sides->add(std::make_shared<Quad>(Vec3(max_x, min_y, max_z), -dz, dy, mat)); 
    sides->add(std::make_shared<Quad>(Vec3(max_x, min_y, min_z), -dx, dy, mat));
    sides->add(std::make_shared<Quad>(Vec3(min_x, min_y, min_z), dz, dy, mat)); 
    sides->add(std::make_shared<Quad>(Vec3(min_x, max_y, max_z), dx, -dz, mat)); 
    sides->add(std::make_shared<Quad>(Vec3(min_x, min_y, min_z), dx, dz, mat)); 

    return sides;
}