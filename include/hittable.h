#ifndef HITTABLE_H
#define HITTABLE_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <OBJ_Loader.h>
#pragma GCC diagnostic pop

#include "input.h"
#include "color.h"
#include "ray.h"
#include "interval.h"
#include "hitrecord.h"
#include "matrix.h"

typedef struct Vertex {
    Vec3f pos;
    Vec3f normal = Vec3f();
} vertex_t;  

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
    Vertex _v0, _v1, _v2; // vertices
    Vec3f _face_normal;
    Color _color;
    Vec3f _v0v1; // axis from v0 to v1 
    Vec3f _v0v2; // axis from v0 to v2

public:
    Triangle() = default;
    //Triangle(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, const Color& col) : 
    //    Triangle(v0, v1, v2, unit_vector(cross(v1-v0, v2-v0)), col) {}
//
    //Triangle(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, const Vec3f& n, const Color& col);
    Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Color&col);

    const Vec3f& get_face_normal() const { return _face_normal; }

    bool hit(const Ray& r_in, HitRecord& hitrec) const;
}; // class Triangle

class Mesh {
private:
    std::vector<objl::Vertex> _vertices;
    std::vector<uint32_t> _indices;
    Color _color;
    std::vector<Triangle> _triangles;
    Mat4 _transf;
    Mat4 _transf_inv;

public:
    Mesh() = default;
    Mesh(const objl::Mesh& mesh, Mat4&& m, Mat4&& m_inv);

    std::vector<Triangle> get_triangles() const { return _triangles; }
}; // class Mesh

class MeshList {
private:
    std::vector<Triangle> _triangles;

public:
    MeshList() = default;

    void add(const objl::Loader& loader, const geometry_params_t& g);

    bool hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const;
}; // class MeshList
#endif