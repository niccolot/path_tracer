#ifndef HITRECORD_H
#define HITRECORD_H

#include "vec3.h"
#include "ray.h"
#include "color.h"

class HitRecord {
private:
    Vec3f _hit_point;
    Vec3f _hitpt_normal;
    float _t;
    Color _color;

public:
    HitRecord() {}
    HitRecord(const Vec3f& hit_pt, const Vec3f& n, float t) :
        _hit_point(hit_pt), _hitpt_normal(n), _t(t) {}

    float get_t() const { return _t; }
    const Vec3f& get_hit_point() const { return _hit_point; }
    const Vec3f& get_normal() const { return _hitpt_normal; }
    const Color& get_color() const { return _color; }

    void set_t(float t) { _t = t; }
    void set_hit_point(const Vec3f& p) { _hit_point = p; }
    //void set_hit_point(Vec3f&& p) { _hit_point = std::move(p); }
    void set_normal(const Vec3f& n, const Vec3f& direction);
    //void set_normal(Vec3f&& n, Vec3f&& direction);
    void set_color(const Color& col) { _color = col; }
    //void set_color(Color&& col) { _color = std::move(col); }
}; // class HitRecord
#endif