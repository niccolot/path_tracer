#include "hittable.h"

void HitRecord::set_face_normal(const Ray& r, const Vec3& outward_normal) {
    // outward_normal is assumed to have unit length

    front_face_val = dot(r.direction(), outward_normal) < 0;
    normal_val = front_face_val ? outward_normal : -outward_normal;
}

bool HittableList::hit(const Ray& r, double ray_tmin, double ray_tmax, HitRecord& rec) const {
    HitRecord temp_rec;
    bool hit_anything = false;
    auto closest_so_far = ray_tmax;

    for (const auto& obj : objects) {
        if (obj->hit(r, ray_tmin, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t();
            rec = temp_rec;
        }
    }
    
    return hit_anything;
}