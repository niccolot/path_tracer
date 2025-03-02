#include "constant_medium.h"

bool ConstantMedium::hit(const Ray& r, Interval ray_t, HitRecord& rec) const {
    HitRecord rec1, rec2;

    if (!boundary->hit(r, Interval::universe(), rec1)) {
        return false;
    }

    if(!boundary->hit(r, Interval(rec1.t() + 0.0001, infinity), rec2)) {
        return false;
    }

    if (rec1.t() < ray_t.min()) rec1.set_t(ray_t.min());
    if (rec2.t() > ray_t.max()) rec2.set_t(ray_t.max());

    if (rec1.t() >= rec2.t()) {
        return false;
    }

    if (rec1.t() < 0) rec1.set_t(0);

    auto ray_length = r.direction().length();
    auto distance_inside_boundary = (rec2.t() - rec1.t()) * ray_length;
    auto hit_distance = neg_inv_density * std::log(RandomUtils::random_double());

    if (hit_distance > distance_inside_boundary) {
        return false;
    }

    rec.set_t(rec1.t() + hit_distance / ray_length);
    rec.set_point(r.at(rec.t()));
    rec.set_normal(Vec3(1,0,0)); // arbitrary
    rec.front_face(); // arbitrary
    rec.set_material(phase_function);

    return true;
}