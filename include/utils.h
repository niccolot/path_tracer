#ifndef UTILS_H
#define UTILS_H

#include <random>

#include "constants.h"

inline double degs_to_rads(double degs) { return degs * pi / 180.0; }

namespace RandomUtils {
    inline std::mt19937 generate_engine() {
        std::random_device rd{};
        std::seed_seq ss{rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd(),};

        return std::mt19937{ ss };
    }

    inline std::mt19937 random_engine{ generate_engine() };

    inline uint32_t xor128() {
        static uint32_t x = 123456789;
        static uint32_t y = 362436069;
        static uint32_t z = 521288629;
        static uint32_t w = 88675123;
        uint32_t t;

        t = x ^ (x << 11);
        x = y; y = z; z = w;
        return w = w ^ (w >> 19) ^ t ^ (t >> 8);
    }

    inline double xor128_double() {
        return xor128() / 4294967296.0;
    } 

    inline double random_double(bool mt = false) {
        /**
         * @brief returns a double in the range [min, max)
         * @param mt wheter to use marsenne twister 
         */
        if (mt) {
            std::uniform_real_distribution<double> distribution(0., 1.);
            return distribution(random_engine);
        } else {
            return xor128_double();
        }
    }

    inline double random_double(double min, double max) {
        return min + (max - min) * random_double();
    }

    inline int random_int(int min, int max) {
        /**
         * @brief returns an int in the range [min, max]
         */
        return int(random_double(min, max+1));
    }
} // namespace RandomUtils
#endif