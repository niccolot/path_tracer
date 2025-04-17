#ifndef MESH_H
#define MESH_H

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
#include "logger.h"
#include "triangle.h"

class Mesh {
private:
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
    std::vector<Mesh> _meshes;
    std::shared_ptr<Logger> _logger;

public:
    MeshList() = default;
    
    void set_logger(std::shared_ptr<Logger> logger) { _logger = logger; }

    void add(const objl::Loader& loader, const geometry_params_t& g);

    bool hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const;
}; // class MeshList
#endif