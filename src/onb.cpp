#include "onb.h"

ONB::ONB(const Vec3& n) {
    /**
     * @brief build an ortonormal base according to 
     * https://orbit.dtu.dk/en/publications/building-an-orthonormal-basis-from-a-3d-unit-vector-without-norma
     */
    axis[2] = n;
    if (n.z() == -1) {
        axis[0] = Vec3(0, 1, 0);
        axis[1] = Vec3(1, 0, 0);
    } else {
        double denom = 1  / (1 + n.z());
        double nxx = n.x() * n.x();
        double nxy = n.x() * n.y();
        double nyy = n.y() * n.y();
        axis[0] = Vec3(1 - nxx * denom, -nxy*denom, -n.x());
        axis[1] = Vec3(-nxy*denom, 1 - nyy*denom, -n.y());
    } 
}

ONB& ONB::operator=(const ONB& base) {
    axis[0] = base.u();
    axis[1] = base.v();
    axis[2] = base.w();

    return *this;
}