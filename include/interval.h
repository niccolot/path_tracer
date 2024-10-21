#ifndef INTERVAL_H
#define INTERVAL_H

#include "constants.h"

class Interval {
    private:
        double min_val, max_val;

    public:
        // empty interval as default
        Interval() : min_val(infinity), max_val(-infinity) {} 
        Interval(double min, double max) : min_val(min), max_val(max) {}
        Interval(const Interval& a, const Interval& b);

        double min() const { return min_val; }
        double max() const { return max_val; }
        void set_min(double min) { min_val = min; }
        void set_max(double max) { max_val = max; }

        static const Interval empty() {return Interval(); }
        static const Interval universe() { return Interval(-infinity, infinity); }

        double size() { return (max_val - min_val); }
        bool contains(double x) { return (min_val <= x && x <= max_val); }
        bool surrounds(double x) { return (min_val < x && x < max_val); }
        double clamp(double x) const;
        Interval expand(double delta) const;
}; // class Interval

#endif