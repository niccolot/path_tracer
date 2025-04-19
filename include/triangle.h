#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "hitrecord.h"
#include "boundingbox.h"

typedef struct Vertex {
    Vec3f pos;
    Vec3f normal;
} vertex_t;  

class Triangle {
private:
    vertex_t _v0, _v1, _v2; // vertices
    Vec3f _face_normal;
    Color _color;
    Vec3f _v0v1; // axis from v0 to v1 
    Vec3f _v0v2; // axis from v0 to v2
    BoundingBox _bbox;

    void _set_bbox();

public:
    Triangle() = default;
    Triangle(const vertex_t& v0, const vertex_t& v1, const vertex_t& v2, const Color&col);

    const vertex_t& v0() const { return _v0; }
    const vertex_t& v1() const { return _v1; }
    const vertex_t& v2() const { return _v2; }

    const Vec3f& get_face_normal() const { return _face_normal; }
    const BoundingBox& get_bbox() const { return _bbox; }

    bool hit(const Ray& r_in, HitRecord& hitrec) const;
}; // class Triangle

inline std::ostream& operator<<(std::ostream& out, const Triangle& v) {
    return out << v.v0().pos << '\n' << v.v1().pos << '\n' << v.v2().pos;
}
#endif