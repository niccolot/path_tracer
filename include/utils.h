#ifndef UTILS_H
#define UTILS_H

#include <random>

#include "constants.h"

inline double degs_to_rads(double degs) { return degs * pi / 180.0; }
double random_double(double min = 0., double max = 1.);
int random_int(int min, int max);

//double random_double(double min = 0, double max = 1) {
//    /**
//     * @brief returns a double in the range [min, max)
//     */
//    static std::uniform_real_distribution<double> distribution(min, max);
//    static std::mt19937 generator;
//
//    return distribution(generator);
//}
//
//int random_int(int min, int max) {
//    /**
//     * @brief returns an int in the range [min, max]
//     */
//
//    return int(random_double(min, max+1));
//}
#endif