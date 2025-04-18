#ifndef GRID_H
#define GRID_H

#include <vector>

#include "vec3.h"
#include "boundingbox.h"
#include "triangle.h"

class Cell {
private:
    std::vector<Triangle> _triangles;
    //BoundingBox _bbox;

public:
    Cell() = default;

    const std::vector<Triangle>& tris() { return _triangles; }

    void push_triangle(const Triangle& tri) { _triangles.push_back(tri); }

    bool hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const;
}; // class Cell

class Grid {
private:
    BoundingBox _bbox; // bbox enclosing the grid
    std::vector<Triangle> _triangles;
    std::vector<Cell> _cells;
    Vec3f _cellsize;
    float _lambda; // hyperparameter that determines the grid resolution
    uint32_t _n[3]; // grid resolution in each dimension
    uint32_t _cell_index[3]; // used for dda traversal
    float _t[3];
    float _dt[3];

    void _insert_triangles();
    bool _dda(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec);
    bool _check_boundary() const;

public:
    Grid() = default;
    Grid(const BoundingBox& bbox, const std::vector<Triangle> tris, float l = 3);

    const BoundingBox& bbox() const { return _bbox; }
    void set_bbox(const BoundingBox& bbox) { _bbox = bbox; } 

    bool hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec);
}; // class Grid
#endif