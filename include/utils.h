#ifndef UTILS_H
#define UTILS_H

#include <random>

#include "constants.h"

inline double degs_to_rads(double degs) { return degs * pi / 180.0; }

inline double random_double(double min = 0, double max = 1) {
    static std::uniform_real_distribution<double> distribution(min, max);
    static std::mt19937 generator;

    return distribution(generator);
}

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