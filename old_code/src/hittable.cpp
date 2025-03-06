#include "hittable.h"

bool HittableList::hit(const Ray& r, Interval ray_t, HitRecord& rec) const {
    HitRecord temp_rec;
    bool hit_anything = false;
    auto closest_so_far = ray_t.max();

    for (const auto& obj : objects) {
        if (obj->hit(r, Interval(ray_t.min(), closest_so_far), temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t();
            rec = temp_rec;
        }
    }
    
    return hit_anything;
}

void HittableList::add(std::shared_ptr<Hittable> object) {
    objects.push_back(object);
    bbox = AxisAlignedBBox(bbox, object->bounding_box());
}

double HittableList::pdf_value(const Vec3& origin, const Vec3& direction) const {
    auto weight = 1.0 / objects.size();
    double sum{};

    for (const auto& obj : objects) {
        sum += weight * obj->pdf_value(origin, direction);
    }

    return sum;
}

Vec3 HittableList::random(const Vec3& origin) const {
    auto int_size = int(objects.size());
    auto vec = objects[RandomUtils::random_int(0, int_size-1)]->random(origin);

    while (vec.is_nan()) {
        vec = objects[RandomUtils::random_int(0, int_size-1)]->random(origin);
    }

    return vec;
}

bool Translate::hit(const Ray& r, Interval ray_t, HitRecord& rec) const {
    Ray offset_r(r.origin() - offset, r.direction(), r.time());

    if(!object->hit(offset_r ,ray_t, rec)) {
        return false;
    }

    rec.set_point(rec.point() + offset);

    return true;
}

RotateY::RotateY(std::shared_ptr<Hittable> object, double angle) : object(object) {

    auto rads = degs_to_rads(angle);
    sin_theta = std::sin(rads);
    cos_theta = std::cos(rads);
    bbox = object->bounding_box();

    Vec3 min(infinity, infinity, infinity);
    Vec3 max(-infinity, -infinity, -infinity);

    for (int i=0; i<2; ++i) {
        for (int j=0; j<2; ++j) {
            for (int k=0; k<2; ++k) {
                auto x = i*bbox.x_axis().max() + (1-i)*bbox.x_axis().min();
                auto y = j*bbox.y_axis().max() + (1-j)*bbox.y_axis().min();
                auto z = k*bbox.z_axis().max() + (1-k)*bbox.z_axis().min();

                auto newx = cos_theta*x + sin_theta*z;
                auto newz = -sin_theta*x + cos_theta*z;

                Vec3 tester(newx, y, newz);

                for (int c=0; c<3; ++c) {
                    min[c] = std::fmin(min[c], tester[c]);
                    max[c] = std::fmax(max[c], tester[c]);
                }
            }
        }
    }

    bbox = AxisAlignedBBox(min, max);
}

bool RotateY::hit(const Ray& r, Interval ray_t, HitRecord& rec) const {
    // transform the ray from global frame to object frame
    auto xo = cos_theta*r.origin().x() - sin_theta*r.origin().z();
    auto yo = r.origin().y();
    auto zo = sin_theta*r.origin().x() + cos_theta*r.origin().z();
    
    auto  origin = Vec3(xo,yo,zo);

    auto xd = cos_theta*r.direction().x() - sin_theta*r.direction().z();
    auto yd = r.direction().y();
    auto zd = sin_theta*r.direction().x() + cos_theta*r.direction().z();

    auto direction = Vec3(xd,yd,zd);

    Ray rotated_r(origin, direction, r.time());

    //check for intersection in object frame
    if (!object->hit(rotated_r, ray_t, rec)) {
        return false;
    }

    // transform the intersection from object frame to global frame
    auto xp = cos_theta*rec.point().x() + sin_theta*rec.point().z();
    auto yp = rec.point().y();
    auto zp = -sin_theta*rec.point().x() + cos_theta*rec.point().z();

    rec.set_point(Vec3(xp,yp,zp));

    auto xn =  cos_theta*rec.normal().x() + sin_theta*rec.normal().z();
    auto yn = rec.normal().y();
    auto zn = -sin_theta*rec.normal().x() + cos_theta*rec.normal().z();

    rec.set_normal(Vec3(xn,yn,zn));

    return true;
}

