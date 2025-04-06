#include "hitrecord.h"

//void HitRecord::set_normal(const Vec3f& n, const Vec3f& direction) {
//    set_normal(n, direction);
//}

void HitRecord::set_normal(const Vec3f& n, const Vec3f& direction) {
    /**
     * @brief: set the outward normal based on the direction
     * of the incoming ray
     */

    bool front_facing = dot(n, direction);
    _hit_point = front_facing ? n : -n;
}