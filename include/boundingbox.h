#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "triangle.h"
#include "interval.h"

class BoundingBox {
private:
    Interval _span_x;
    Interval _span_y;
    Interval _span_z;
    float _volume;
    std::vector<Vec3f> _bounds;
    std::vector<Triangle> _triangles;

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
    const std::vector<Vec3f>& bounds() const { return _bounds; }
    uint32_t num_tris() const { return _triangles.size(); }

    bool hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const;
}; // class BoundginBox

#endif