#include "quad.h"

Quad::Quad(
    const Vec3& Q,
    const Vec3& u,
    const Vec3& v,
    std::shared_ptr<Material> mat) 
        : Q(Q), u(u), v(v), mat(mat) {

    normal = unit_vector(cross(u,v));
    D = dot(normal, Q);
    w = normal / dot(normal, normal);
    set_bounding_box();
}

bool Quad::hit(const Ray& r, Interval ray_t, HitRecord& rec) const {
    auto denom = dot(normal, r.direction());

    // ray (almost) parallel to plane
    if (std::fabs(denom) < 1e-8) {
        return false;
    }

    // hitpoint outside ray interval
    auto t = (D - dot(normal, r.origin())) / denom;
    if (!ray_t.contains(t)) {
        return false;
    }

    auto intersection = r.at(t);
    Vec3 planar_hitpt_vec = intersection - Q;
    auto alpha = dot(w, cross(planar_hitpt_vec, v));
    auto beta = dot(w, cross(u, planar_hitpt_vec));

    //hitpoint outside quadrilateral
    if (!is_interior(alpha, beta, rec)) {
        return false;
    }

    rec.set_t(t);
    rec.set_point(intersection);
    rec.set_material(mat);
    rec.set_face_normal(r, normal);

    return true;
}

void Quad::set_bounding_box() {
    auto bbox_diagonal1 = AxisAlignedBBox(Q, Q+u+v);
    auto bbox_diagonal2 = AxisAlignedBBox(Q+u, Q+v);
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