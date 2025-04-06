#ifndef HITTABLE_H
#define HITTABLE_H

#include "color.h"
#include "ray.h"
#include "interval.h"
#include "hitrecord.h"

class Sphere {
private:
    Vec3f _center;
    float _radius;
    Color _color;

public:
    Sphere() = delete;
    Sphere(Vec3f center, float r, Color col) : 
        _center(center), _radius(r), _color(col) {}

    bool hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const;
}; // class Sphere
#endif