#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

using Color = Vec3;

inline double linear_to_gamma(double linear_component) {
    /**
     * @brief gamma2 transform
     */
    if (linear_component > 0) {
        return std::sqrt(linear_component);
    }

    return 0;
}
#endif