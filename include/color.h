#ifndef COLOR_H
#define COLOR_H

#include <cmath>

#include "vec3.h"

using Color = Vec3f;

inline float linear_to_gamma(float component) {
    if (component <= 0.f) {
        return 0.f;
    } 

    return std::sqrt(component);
}
#endif