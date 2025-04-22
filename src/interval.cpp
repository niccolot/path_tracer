#include "interval.h"

Interval::Interval(const Interval& a, const Interval& b) {
    /**
     * @brief construct an interval from the lower min and
     * higher max encompassing everything in the middle
     */
    _min = b.min() <= a.min() ? b.min() : a.min();
    _max = b.max() >= a.max() ? b.max() : a.max();
}

float Interval::clamp(float x) const {
    /**
     * @brief returns an x that lies in [_min, _max]
     */
    if ( x < _min) {
        return _min;
    } else if (x > _max) {
        return _max;
    } else {
        return x;
    }
}

Interval Interval::expand(float delta) {
    _min = _min - 0.5f * delta;
    _max = _max + 0.5f * delta;

    return *this;
}
