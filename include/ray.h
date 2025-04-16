#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class Ray {

private:
    Vec3f _origin, _direction, _inv_dir;
    float _t;
    bool _sign[3];

public:
    Ray() = delete;
    Ray(const Vec3f& o, const Vec3f& d);

    const Vec3f& origin() const { return _origin; }
    const Vec3f& direction() const { return _direction; }
    const Vec3f& inv_dir() const { return _inv_dir; }
    float time() const { return _t; }
    Vec3f at(float t) const { return _origin + t * _direction; }
    bool sign_x() const { return _sign[0]; }
    bool sign_y() const { return _sign[1]; }
    bool sign_z() const { return _sign[2]; } 
}; // class Ray
#endif