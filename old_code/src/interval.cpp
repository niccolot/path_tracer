#include "interval.h"

Interval::Interval(const Interval& a, const Interval& b) {
    min_val = b.min() <= a.min() ? b.min() : a.min();
    max_val = b.max() >= a.max() ? b.max() : a.max();
}

double Interval::clamp(double x) const {
    if (x < min_val) {
        return min_val;
    } else if (x > max_val) {
        return max_val;
    } else {
        return x;
    }
}

Interval Interval::expand(double delta) const {
    auto padding = 0.5*delta;
    
    return Interval(min_val - padding, max_val + padding); 
}

Interval operator+(const Interval& i, double delta) {
    return Interval(i.min() + delta, i.max() + delta);
}

Interval operator+(double delta, const Interval& i) {
    return i + delta;
}