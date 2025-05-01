#include "boundingbox.h"

BoundingBox::BoundingBox(const Vec3f& p_min, const Vec3f& p_max) {
    /**
     * @details: it pads every direction by a delta in order
     * to avoid zero dimensional sides and to be slightly 
     * larger than the object enclosed by the bbox
     */
    const float delta = 0.0001;
    _bounds.resize(2);
    _bounds[0] = p_min - 0.5 * delta;
    _bounds[1] = p_max + 0.5 * delta;
    _span_x = Interval(p_min.x(), p_max.x());
    _span_y = Interval(p_min.y(), p_max.y());
    _span_z = Interval(p_min.z(), p_max.z());
    _span_x.expand(delta);
    _span_y.expand(delta);
    _span_z.expand(delta);
    _volume = _span_x.size() * _span_y.size() * _span_z.size();
}

bool BoundingBox::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const {
    /**
     * @brief: ray-bounding box intersection algorithm
     */
    float t_min = (_bounds[r_in.sign_x()].x() - r_in.origin().x()) * r_in.inv_dir().x();
    float t_max = (_bounds[1 - r_in.sign_x()].x() - r_in.origin().x()) * r_in.inv_dir().x();
    float ty_min = (_bounds[r_in.sign_y()].y() - r_in.origin().y()) * r_in.inv_dir().y();
    float ty_max = (_bounds[1 - r_in.sign_y()].y() - r_in.origin().y()) * r_in.inv_dir().y();

    if ((t_min > ty_max) || (ty_min > t_max)) {
        return false;
    }

    t_min = t_min > ty_min ? t_min : ty_min;
    t_max = t_max < ty_max ? t_max : ty_max;

    float tz_min = (_bounds[r_in.sign_z()].z() - r_in.origin().z()) * r_in.inv_dir().z();
    float tz_max = (_bounds[1 - r_in.sign_z()].z() - r_in.origin().z()) * r_in.inv_dir().z();

    if ((t_min > tz_max) || (tz_min > t_max)) {
        return false;
    }

    t_min = t_min > tz_min ? t_min : tz_min;
    t_max = t_max < tz_max ? t_max : tz_max;

    t_min = t_min > 0 ? t_min : t_max;
    if (t_min < 0) {
            return false;
    }

    if (!ray_t.contains(t_min)) {
        return false;
    }

    hitrec.set_t(t_min); 

    return true;
}