#include "ray.h"

Ray::Ray(const Vec3f& o, const Vec3f& d) 
: _origin(o), _direction(unit_vector(d)), _t(0.f) 
{
    Vec3f _inv_dir = 1.f / _direction;
    _sign[0] = (_inv_dir.x() < 0);
    _sign[1] = (_inv_dir.y() < 0);
    _sign[2] = (_inv_dir.z() < 0);
}