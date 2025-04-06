#ifndef UTILS_H
#define UTILS_H

#include <numbers>

inline float degs_to_rads(float d) {
    return d * std::numbers::pi / 180.f;
}

#endif