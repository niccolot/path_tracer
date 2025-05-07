#ifndef GRID_H
#define GRID_H

#include <vector>
#include <memory>

#include "vec3.h"
#include "boundingbox.h"
#include "triangle.h"
#include "logger.h"

class Grid;

class Cell {
private:
    std::vector<Triangle> _triangles;

public:
    Cell() = default;
    const std::vector<Triangle>& tris() { return _triangles; }

    void push_triangle(const Triangle& tri) { _triangles.push_back(tri); }

    uint32_t num_tris() const { return _triangles.size(); }

    bool hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec, Grid& g) const;
}; // class Cell

class Grid {
private:
    BoundingBox _bbox; // bbox enclosing the grid
    std::vector<Triangle> _triangles;
    std::shared_ptr<Logger> _logger;
    std::vector<Cell> _cells;
    Vec3f _cellsize;
    float _lambda; // hyperparameter that determines the grid resolution
    uint32_t _n[3]{}; // grid resolution in each dimension
    int32_t _cell_index[3]; // used for dda traversal

    void _insert_triangles();
    bool _dda(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec);

public:
    Grid() = default;
    Grid(const BoundingBox& bbox, const std::vector<Triangle> tris, std::shared_ptr<Logger> logger, float lambda = 5);

    const BoundingBox& bbox() const { return _bbox; }
    void set_bbox(const BoundingBox& bbox) { _bbox = bbox; } 

    bool hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec);

    friend Cell;
}; // class Grid
#endif