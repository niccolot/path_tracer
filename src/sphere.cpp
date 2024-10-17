#include "sphere.h"

bool Sphere::hit(const Ray& r, Interval ray_t, HitRecord& rec) const {
    Vec3 oc = center - r.origin();
    auto a = r.direction().length_squared();  
    auto h = dot(r.direction(), oc); // h = -b/2 in 2nd order equation roots formula
    auto c = oc.length_squared() - radius*radius;

    auto discriminant = h*h - a*c;
    if (discriminant < 0) {
        return false;
    }

    auto sqrt_delta = std::sqrt(discriminant);

    // find the nearest root that lies in the acceptable range [ray_tmin, ray_tmax]
    auto root = (h - sqrt_delta) / a;
    if (!ray_t.surrounds(root)) {
        root = (h + sqrt_delta) / a;
        if(!ray_t.surrounds(root)) {
            return false;
        }
    }

    rec.set_t(root);
    rec.set_point(r.at(root));
    
    Vec3 outward_normal = (rec.point() - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.set_material(mat);

    return true;
}