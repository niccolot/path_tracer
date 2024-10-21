#ifndef AXIS_ALIGNED_BBOX_H
#define AZISI_ALIGNED_BBOX_H

#include "vec3.h"
#include "interval.h"
#include "ray.h"

class AxisAlignedBBox {
    private:
        Interval x, y, z;
    
    public:
        AxisAlignedBBox() {}
        AxisAlignedBBox(const Vec3& a, const Vec3& b);
        AxisAlignedBBox(const AxisAlignedBBox& box0, const AxisAlignedBBox& box1);
        AxisAlignedBBox(const Interval& x, const Interval& y, const Interval& z) :
            x(x), y(y), z(z) {}
        
        const Interval& axis_interval(int n) const;
        const Interval& x_axis() const { return x; }
        const Interval& y_axis() const { return y; }
        const Interval& z_axis() const { return z; }
        int longest_axis() const;
        static const AxisAlignedBBox empty();
        static const AxisAlignedBBox universe();
        bool hit(const Ray& r, Interval ray_t) const;
}; // class AxisAlignedBbox
#endif