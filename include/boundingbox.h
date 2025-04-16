#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "interval.h"
#include "hittable.h"

class BoundingBox {
private:
    Interval _span_x;
    Interval _span_y;
    Interval _span_z;

public:
    BoundingBox() = default;
    BoundingBox(const Vec3f& p_min, const Vec3f& p_max);
}; // class BoundginBox

#endif