#ifndef INTERVAL_H
#define INTERVAL_H

#include <limits>

const float inf = std::numeric_limits<float>::infinity();

class Interval {
private:
    float _min, _max;

public:
    // empty interval
    Interval() : _min(inf), _max( inf) {}

    Interval(float min, float max) : _min(min), _max(max) {}
    Interval(const Interval& a, const Interval& b);

    float min() const { return _min; }
    float max() const { return _max; }
    void set_min(float min) { _min = min; }
    void set_max(float max) { _max = max; }

    static const Interval empty() { return Interval(); }
    static const Interval universe() { return Interval(-inf, inf); }

    float size() const { return (_max - _min); }
    bool contains(float x) const { return (_min <= x && x <= _max); }
    bool surrounds(float x) const { return (_min < x && x < _max); }
    float clamp(float x) const;
    Interval expand(float delta);
}; // class Interval

inline Interval operator+(const Interval& i, float a) {
    /**
     * @brief shifts the interval 
     */
    return Interval(i.min() + a, i.max() + a);
}

inline Interval operator+(float a, const Interval& i) { return i + a; }
inline Interval operator-(const Interval& i, float a) { return i + (-a); }
inline Interval operator-(float a, const Interval& i) {return i - a; }
#endif