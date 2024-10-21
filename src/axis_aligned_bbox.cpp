#include "axis_aligned_bbox.h"

AxisAlignedBBox::AxisAlignedBBox(const Vec3& a, const Vec3& b) {
    x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
    y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
    z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);
}

AxisAlignedBBox::AxisAlignedBBox(
    const AxisAlignedBBox& box0, 
    const AxisAlignedBBox& box1) {
    
    x = Interval(box0.x_axis(), box1.x_axis());
    y = Interval(box0.y_axis(), box1.y_axis());
    z = Interval(box0.z_axis(), box1.z_axis());
}

const Interval& AxisAlignedBBox::axis_interval(int n) const {
    switch (n){
    case 0:
        return x;    
    case 1:
        return y;    
    case 2:
        return z;
    default:
        throw std::out_of_range("Invalid axis index, must be 0, 1 or 2");
    }
}

bool AxisAlignedBBox::hit(const Ray& r, Interval ray_t) const {
    const Vec3& ray_origin = r.origin();
    const Vec3& ray_dir = r.direction();

    for (int axis=0; axis<3; ++axis) {
        const Interval& ax = axis_interval(axis);
        const double adinv = 1. / ray_dir[axis];

        auto t0 = (ax.min() - ray_origin[axis])*adinv;
        auto t1 = (ax.max() - ray_origin[axis])*adinv;

        if (t0 < t1) {
            if (t0 > ray_t.min()) ray_t.set_min(t0);
            if (t1 < ray_t.max()) ray_t.set_max(t1);
        } else {
            if (t1 > ray_t.min()) ray_t.set_min(t1);
            if (t0 < ray_t.max()) ray_t.set_max(t0);
        }

        if (ray_t.max() <= ray_t.min()) {
            return false;
        }
    }

    return true;
}