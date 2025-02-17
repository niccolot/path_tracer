#include "utils.h"

double random_double(double min, double max) {
    /**
     * @brief returns a double in the range [min, max)
     */
    static std::uniform_real_distribution<double> distribution(min, max);
    static std::mt19937 generator;

    return distribution(generator);
}

int random_int(int min, int max) {
    /**
     * @brief returns an int in the range [min, max]
     */

    return int(random_double(min, max+1));
}