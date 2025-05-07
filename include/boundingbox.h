#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include  <array>

#include "interval.h"
#include "ray.h"
#include "hitrecord.h"

class BoundingBox {
private:
    float _volume;
    Interval _span_x;
    Interval _span_y;
    Interval _span_z;
    std::array<Vec3f, 2> _bounds;

public:
    BoundingBox() = default;
    BoundingBox(const Vec3f& p_min, const Vec3f& p_max);

    const Interval& span_x() const { return _span_x; }
    const Interval& span_y() const { return _span_y; }
    const Interval& span_z() const { return _span_z; }
    float size_x() const { return _span_x.size(); }
    float size_y() const { return _span_y.size(); }
    float size_z() const { return _span_z.size(); }
    float volume() const { return _volume; }
    const std::array<Vec3f, 2>& bounds() const { return _bounds; }

    bool hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const;
}; // class BoundingBox

#endif