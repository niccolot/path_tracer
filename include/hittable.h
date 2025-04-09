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


class Triangle {
private:
    Vec3f _v0, _v1, _v2; // vertices
    Color _color;
    Vec3f _v0v1; // axis from v0 to v1 
    Vec3f _v0v2; // axis from v0 to v2

public:
    Triangle() = delete;
    Triangle(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, const Color& col);
    bool hit(const Ray& r_in, [[maybe_unused]] const Interval& ray_t, HitRecord& hitrec) const;
}; // class Triangle
#endif