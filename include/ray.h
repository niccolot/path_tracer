#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class Ray {

private:
    Vec3f _origin, _direction;
    float _t;

public:
    Ray() = delete;
    Ray(const Vec3f& o, const Vec3f& d) : 
        _origin(o), _direction(d), _t(0.f) {}

    const Vec3f& origin() const { return _origin; }
    const Vec3f& direction() const { return _direction; }
    float time() const { return _t; }
    const Vec3f at(float t) const { return _origin + t * _direction; } 
}; // class Ray
#endif