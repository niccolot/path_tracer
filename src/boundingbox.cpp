#include "boundingbox.h"

BoundingBox::BoundingBox(const Vec3f& p_min, const Vec3f& p_max) {
    _span_x = Interval(p_min.x(), p_max.x());
    _span_y = Interval(p_min.y(), p_max.y());
    _span_z = Interval(p_min.z(), p_max.z());
}