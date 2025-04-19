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
#include "grid.h"

class Mesh {
private:
    Color _color;
    std::vector<Triangle> _triangles;
    Mat4 _transf;
    Mat4 _transf_inv;
    mutable Grid _grid;

public:
    Mesh() = default;
    Mesh(const objl::Mesh& mesh, Mat4&& m, Mat4&& m_inv);

    const std::vector<Triangle>& get_triangles() const { return _triangles; }
    Grid& grid() { return _grid; } 

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