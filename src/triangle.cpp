#include "triangle.h"

Triangle::Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Color&col) {
    _v0 = v0;
    _v1 = v1;
    _v2 = v2;
    _v0v1 = v1.pos - v0.pos;
    _v0v2 = v2.pos - v0.pos;
    _face_normal = unit_vector(_v0.normal + _v1.normal + _v2.normal);
    _color = col;
}

bool Triangle::hit(const Ray& r_in, HitRecord& hitrec) const {
    /**
     * @brief: ray-triangle intersection method using moller-trumbore algorithm
     */
    const float tol = 1e-8;

    Vec3f p_vec = cross(r_in.direction(), _v0v2);
    float det = dot(_v0v1, p_vec);

    // use std::fabs(det) < tol to disable backface culling
    if (det < tol) {
        return false;
    }

    float det_inv = 1.f / det;
    Vec3f t_vec = r_in.origin() - _v0.pos;
    float u = dot(t_vec, p_vec) * det_inv;
    if (u < 0 || u > 1) {
        return false;
    }

    Vec3f q_vec = cross(t_vec, _v0v1);
    float v = dot(r_in.direction(), q_vec) * det_inv;
    if (v < 0 || u + v > 1) {
        return false;
    }
    
    float t = dot(_v0v2, q_vec) * det_inv;
    hitrec.set_u(u);
    hitrec.set_v(v);
    hitrec.set_t(t);
    hitrec.set_hit_point(r_in.at(t));
    hitrec.set_normal((1.f - u - v) * _v0.normal + u * _v1.normal + v * _v2.normal);
    hitrec.set_color(_color * std::fmax(0.f, -dot(hitrec.get_normal(), r_in.direction())));

    return true;
}