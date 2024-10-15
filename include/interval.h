#ifndef INTERVAL_H
#define INTERVAL_H

#include "constants.h"

class Interval {
    private:
        double min_val, max_val;

    public:
        Interval() : min_val(infinity), max_val(-infinity) {} // empty interval as default
        Interval(double min, double max) : min_val(min), max_val(max) {}

        double min() const { return min_val; }
        double max() const { return max_val; }

        static const Interval empty() {return Interval(); }
        static const Interval universe() { return Interval(-infinity, infinity); }

        double size() { return (max_val - min_val); }
        bool contains(double x) { return (min_val <= x && x <= max_val); }
        bool surrounds(double x) { return (min_val < x && x < max_val); }
        double clamp(double x) const {
            if (x < min_val) {
                return min_val;
            } else if (x > max_val) {
                return max_val;
            } else {
                return x;
            }
        }
}; // class Interval

#endif