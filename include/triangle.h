#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "hitrecord.h"

typedef struct Vertex {
    Vec3f pos;
    Vec3f normal;
} vertex_t;  

class Triangle {
private:
    Vertex _v0, _v1, _v2; // vertices
    Vec3f _face_normal;
    Color _color;
    Vec3f _v0v1; // axis from v0 to v1 
    Vec3f _v0v2; // axis from v0 to v2

public:
    Triangle() = default;
    Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Color&col);

    const Vec3f& get_face_normal() const { return _face_normal; }

    bool hit(const Ray& r_in, HitRecord& hitrec) const;
}; // class Triangle
#endif