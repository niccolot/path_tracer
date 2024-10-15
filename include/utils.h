#ifndef UTILS_H
#define UTILS_H

#include <random>
#include "constants.h"
#include "vec3.h"

inline double degs_to_rads(double degs) { return degs * pi / 180.0; }

inline double random_double(double min, double max) {
    static std::uniform_real_distribution<double> distribution(min, max);
    static std::mt19937 generator;

    return distribution(generator);
}

inline Vec3 sample_square() {
    /**
     * @brief returns a Vec3 randomly sampled in the [-0.5, 0.5] X [-0.5, 0.5]
     * unit square
     */

    return Vec3(random_double(-0.5, 0.5), random_double(-0.5, 0.5), 0);
}
#endif