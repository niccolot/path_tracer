#include "onb.h"

ONB::ONB(const Vec3& n) {
    axis[2] = unit_vector(n);
    Vec3 a = (std::fabs(axis[2].x()) > 0.9) ? Vec3(0,1,0) : Vec3(1,0,0);
    axis[1] = unit_vector(cross(axis[2], a));
    axis[0] = cross(axis[2], axis[1]);
}