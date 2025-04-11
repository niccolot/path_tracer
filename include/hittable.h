#ifndef HITTABLE_H
#define HITTABLE_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <OBJ_Loader.h>
#pragma GCC diagnostic pop

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
    Vec3f _face_normal;
    Color _color;
    Vec3f _v0v1; // axis from v0 to v1 
    Vec3f _v0v2; // axis from v0 to v2

public:
    Triangle() = default;
    Triangle(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, const Color& col) : 
        Triangle(v0, v1, v2, unit_vector(cross(v0-v1, v0-v2)), col) {}

    Triangle(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, const Vec3f& n, const Color& col);

    Vec3f get_face_normal() const { return _face_normal; }

    bool hit(const Ray& r_in, HitRecord& hitrec) const;
}; // class Triangle

class Mesh {
private:
    std::vector<objl::Vertex> _vertices;
    std::vector<uint32_t> _indices;
    Color _color;
    std::vector<Triangle> _triangles;

public:
    Mesh() = default;
    Mesh(const objl::Mesh& mesh);

    std::vector<Triangle> get_triangles() const { return _triangles; }
}; // class Mesh

class MeshList {
private:
    std::vector<Mesh> _mesh_list;
    std::vector<Triangle> _triangles;

public:
    MeshList() = default;
    MeshList(const objl::Loader& loader);

    void add(const objl::Loader& loader);

    const std::vector<Mesh>& get_meshes() const { return _mesh_list; }

    bool hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const;
}; // class MeshList
#endif