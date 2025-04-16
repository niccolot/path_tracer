#ifndef HITTABLE_H
#define HITTABLE_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <OBJ_Loader.h>
#pragma GCC diagnostic pop

#include <memory>

#include "input.h"
#include "color.h"
#include "ray.h"
#include "interval.h"
#include "hitrecord.h"
#include "matrix.h"
#include "boundingbox.h"
#include "logger.h"

typedef struct Vertex {
    Vec3f pos;
    Vec3f normal;
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
    Vec3f _p_min, _p_max;
    std::vector<Vec3f> _bounds;

    void _set_min_max(const Vec3f& v);

public:
    Mesh() = default;
    Mesh(const objl::Mesh& mesh, Mat4&& m, Mat4&& m_inv);

    const std::vector<Triangle>& get_triangles() const { return _triangles; }
    const Vec3f& get_p_min() const { return _p_min; }
    const Vec3f& get_p_max() const { return _p_max; }

    bool hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const;
}; // class Mesh

class MeshList {
private:
    std::vector<Triangle> _triangles;
    std::vector<Mesh> _meshes;
    std::shared_ptr<Logger> _logger;

public:
    MeshList() = default;
    
    void set_logger(std::shared_ptr<Logger> logger) { _logger = logger; }

    void add(const objl::Loader& loader, const geometry_params_t& g);

    bool hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const;
}; // class MeshList
#endif