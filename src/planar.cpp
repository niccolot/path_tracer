#include "planar.h"

Planar::Planar(
    const Vec3& Q,
    const Vec3& u,
    const Vec3& v,
    std::shared_ptr<Material> mat) 
        : _Q(Q), _u(u), _v(v), mat(mat) {

    auto n = cross(_u, _v);
    _normal = unit_vector(n);
    D = dot(_normal, _Q);
    w = n / dot(n,n);
    area = n.length();
}

bool Planar::hit(const Ray& r, Interval ray_t, HitRecord& rec) const {
    auto denom = dot(_normal, r.direction());

    // ray (almost) parallel to plane
    if (std::fabs(denom) < 1e-8) {
        return false;
    }

    // hitpoint outside ray interval
    auto t = (D - dot(_normal, r.origin())) / denom;
    if (!ray_t.contains(t)) {
        return false;
    }

    auto intersection = r.at(t);
    Vec3 planar_hitpt_vec = intersection - _Q;
    double alpha{};
    double beta{};

    muller_trumbore_intersect(alpha, beta, planar_hitpt_vec);

    //hitpoint outside the actual figure
    if (!is_interior(alpha, beta, rec)) {
        return false;
    }

    rec.set_t(t);
    rec.set_point(intersection);
    rec.set_material(mat);
    rec.set_face_normal(r, _normal);

    return true;
}

double Planar::pdf_value(const Vec3& origin, const Vec3& direction) const {
    HitRecord rec;
    if (!this->hit(Ray(origin, direction), Interval(0.001, infinity), rec)) {
        return 0;
    }

    auto distance_squared = rec.t() * rec.t() * direction.length_squared();
    auto cosine = std::fabs(dot(direction, rec.normal()) / direction.length());

    return distance_squared / (cosine * area);
}

void Planar::muller_trumbore_intersect(double& alpha, double& beta, const Vec3& planar_hitpt_vec) const {
    /**
     * @brief calculates intersection parameters alpha and beta 
     * (that are used to check if a ray is inside the planar figure)
     * using muller trombore algorithm, which is basically cramers rule
     */

    double delta;
    if ((delta = _u.x() * _v.y() - _u.y()*_v.x()) != 0) {

        alpha = (planar_hitpt_vec.x() * _v.y() - planar_hitpt_vec.y() * _v.x()) / delta;
        beta = (planar_hitpt_vec.y() * _u.x() - planar_hitpt_vec.x() * _u.y()) / delta;

    } else if ((delta = _u.x() * _v.z() - _u.z() * _v.x()) != 0) {

        alpha = (planar_hitpt_vec.x() * _v.z() - planar_hitpt_vec.z() * _v.x()) / delta;
        beta = (planar_hitpt_vec.z() * _u.x() - planar_hitpt_vec.x() * _u.z()) / delta;

    } else {

        delta = _u.y() * _v.z() - _u.z() * _v.y();
        alpha = (planar_hitpt_vec.y() * _v.z() - planar_hitpt_vec.z() * _v.y()) / delta;
        beta = (planar_hitpt_vec.z() * _u.y() - planar_hitpt_vec.y() * _u.z()) / delta;
    }
}