#ifndef RANDOM_H
#define RANDOM_H

#include <random>

namespace RandomUtils {
    inline std::mt19937 generate_engine() {
        std::random_device rd{};
        std::seed_seq ss{rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};

        return std::mt19937{ss};
    }

    inline std::mt19937 random_engine{ generate_engine() };

    inline uint32_t xor128() {
        /**
         * @brief xor128 algorithm for uint32_t
         * 
         * note that is not seeded so it will 
         * produce always the same sequence, not 
         * a problem per se in raytracing
         */

        static uint32_t x = 123456789;
        static uint32_t y = 362436069;
        static uint32_t z = 521288629;
        static uint32_t w = 88675123;
        uint32_t t;

        t = x ^ (x << 11);
        x = y; y = z; z = w;

        return w = w ^ (w >> 19) ^ t ^ (t >> 8);
    }

    inline float xor128_float() {
        return xor128() / 4294967296.0f;
    }

    inline float random_float(bool mt = false) {
        /**
         * @brief returns a double in [0, 1)
         * @param mt wheter to use marsenne twister 
         * for better precison but less speed
         */
        if (mt) {
            std::uniform_real_distribution<float> distribution(0., 1.);
            return distribution(random_engine);
        } else {
            return xor128_float();
        }
    }

    inline float random_float(float min, float max, bool mt = false) {
        /**
         * @brief returns a float in [min, max)
         */
        return min + (max - min) * random_float(mt);
    }

    inline uint32_t random_int(uint32_t min, uint32_t max, bool mt = false) {
        /**
         * @brief returns an uint32_t in the range [min, max]
         */

        return uint32_t(random_float(min, max, mt));
    }
} // namespace RandomUtils

#endif