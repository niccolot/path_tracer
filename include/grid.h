#ifndef GRID_H
#define GRID_H

#include <vector>

#include "vec3.h"
#include "boundingbox.h"

class Cell {
private:
    std::vector<Triangle> _triangles;

public:
    Cell() = default;
    Cell(const std::vector<Triangle>& tris);

    void push_triangle(const Triangle& tri) { _triangles.push_back(tri); }

    bool hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const;
}; // class Cell

class Grid {
private:
    BoundingBox _bbox; // bbox enclosing the grid
    float _cellsize_x, _cellsize_y, _cellsize_z;
    float _lambda; // hyperparameter that determines the grid resolution
    uint32_t _nx, _ny, _nz; // number of cells in each dimension
    std::vector<Cell> _cells;
    uint32_t _cell_index[2];

    void _dda(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec);
    bool check_boundary() const;
    
public:
    Grid() = default;
    Grid(const BoundingBox& bbox, float l = 3.5);

    bool hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const;
}; // class grid
#endif